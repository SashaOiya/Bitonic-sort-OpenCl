#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "../include/ocl.hpp"

#define CHECK_ERR(err, msg) \
    if (err != CL_SUCCESS) { \
        std::cerr << "Ошибка: " << msg << " (" << err << ")" << std::endl; \
        exit(1); \
    }


using T = int;

size_t next_power_of_two ( const size_t n )
{
    return std::pow(2, std::ceil(std::log2(n)));
}

int compare(const void* a, const void* b) {
    int int_a = *(int*)a;
    int int_b = *(int*)b;
    
    if (int_a < int_b) return -1;
    if (int_a > int_b) return 1;
    return 0;
}

void get_result ( OCL::OclApp<T> app, std::size_t new_size )
{

    CHECK_ERR(app.kernel().setArg(0, app.buffer()), "Ошибка в setArg(0" );
    CHECK_ERR(app.kernel().setArg(1, static_cast<int>(new_size)), "Ошибка в setArg(1)" );

    for (int stage = 2; stage <= new_size; stage *= 2) {
        for (int step = stage / 2; step > 0; step /= 2) {
            app.kernel().setArg(2, stage);
            app.kernel().setArg(3, step);
            CHECK_ERR(app.queue().enqueueNDRangeKernel(app.kernel(), cl::NullRange,  cl::NDRange(new_size), cl::NullRange),  "enqueueNDRangeKernel");
            app.queue().finish();
        }
    }
}

void get_answer ( const std::string& filename)
{
    std::vector<int> ans = {};
    std::ifstream file ( filename );
    if (!file) {
        std::cout << "error\n";
        exit(1);
    }

    int data_size = 0;
    file >> data_size;
    for ( int i = 0, element = 0; i < data_size; ++i ) {
        file >> element;
        ans.push_back ( element );
    }

    std::sort( ans.begin(), ans.end() );
}

static void BM_BitSort(benchmark::State& state) {
    const std::string path = "../tests/test3.txt";
    std::vector<int> res = {};
    std::ifstream file ( path );
    if (!file) {
        std::cout << "error\n";
        exit(1);
    }

    std::size_t original_size = 0;
    file >> original_size;
    std::vector<T> data = {};
    for ( int i = 0, element = 0; i < original_size; ++i ) {
        file >> element;
        data.push_back ( element );
    }

    std::size_t new_size = next_power_of_two ( original_size );
    data.resize ( new_size, std::numeric_limits<T>::max() );

    const std::string kernel_path = "../kernels/bitonic_sort.cl";
    OCL::OclApp<T> app { kernel_path, "bitonicSort", data };

    for (auto _ : state) {
        get_result ( app, new_size );
    }

    res.resize ( new_size, 0);
    CHECK_ERR(app.queue().enqueueReadBuffer(app.buffer(), CL_TRUE, 0, sizeof(T) * new_size, res.data()), "enqueueReadBuffer");
}

static void BM_Sort(benchmark::State& state) {
    for (auto _ : state) {
        const std::string path = "../tests/test3.txt";
        get_answer ( path );
    }
}

BENCHMARK(BM_BitSort)->Iterations( 1 );
BENCHMARK(BM_Sort)->Iterations( 1 );

BENCHMARK_MAIN();