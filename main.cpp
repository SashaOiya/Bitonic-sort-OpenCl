#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits>

#include "include/ocl.hpp"
#include "include/bitsort.hpp"

void printArray(const std::vector<int>& arr, int size) {
    for (int i = 0; i < size; i++) std::cout << arr[i] << " ";
    std::cout << std::endl;
}

int next_power_of_two(int n) {
    return std::pow ( 2, std::ceil(std::log2(n)) );
}

int main() try
{
    std::size_t original_size = 0;
    std::cin >> original_size;
    if ( original_size <= 0 ) { throw std::runtime_error ( "Error: invalid size\n"); }

    std::vector<int> data = {};
    for ( int i = 0, element = 0; i < original_size; ++i ) {
        std::cin >> element;
        if ( !std::cin.good() ) { throw std::runtime_error ( "Error : invalid argumnet\n" ); }
        data.push_back ( element );
    }

    std::size_t new_size = next_power_of_two ( original_size );
    data.resize ( new_size, std::numeric_limits<int>::max() );

    //const OCL::OclPlatform platform = {};
    OCL::OclApp<int> app { "kernels/bitonic_sort.cl", "bitonicSort", data }; 
    BitonicSort::GpuBitonicSort<int> bsrt = { app, data  };
    
    auto start = std::chrono::high_resolution_clock::now();
    bsrt.sort( data );
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed_time = std::chrono::duration<double, std::milli>(end - start).count();

    printArray(data, original_size);

    std::cout << "Время работы на GPU: " << elapsed_time << " мс" << std::endl;

    return 0;
}
catch(const std::exception& exceptions )
{
    std::cerr << exceptions.what() << std::endl;
}