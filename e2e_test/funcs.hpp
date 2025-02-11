#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <gtest/gtest.h>

#include "ocl.hpp"

#define CHECK_ERR(err, msg) \
    if (err != CL_SUCCESS) { \
        std::cerr << "Ошибка: " << msg << " (" << err << ")" << std::endl; \
        exit(1); \
    }

namespace test_funcs
{
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

	void get_result ( const std::string& filename, std::vector<int>& res, const std::string& kernelpath )
    {
        std::ifstream file ( filename );
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

        OCL::OclApp<T> app { kernelpath, "bitonicSort", data };

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
        res.resize ( new_size, 0);
        CHECK_ERR(app.queue().enqueueReadBuffer(app.buffer(), CL_TRUE, 0, sizeof(T) * new_size, res.data()), "enqueueReadBuffer");
        
    }

    void get_answer ( const std::string& filename, std::vector<T>& ans )
    {
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

	void run_test (const std::string& test_name)
	{
		std::string test_directory = "/tests";
		std::string test_path = std::string(TEST_DATA_DIR) + test_directory + test_name;
        std::string kernel = "/kernels/bitonic_sort.cl";
        std::string kernel_path = std::string(TEST_DATA_DIR) + kernel;

        std::vector<T> res;
		get_result(test_path, res, kernel_path );

        std::vector<T> ans;
		get_answer(test_path, ans);

        //EXPECT_TRUE(res.size() == ans.size());
        for (int i = 0; i < ans.size(); i++)
        {
            EXPECT_EQ(res[i], ans[i]);
        }
	}
}