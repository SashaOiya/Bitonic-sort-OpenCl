#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits>

#include "include/ocl.hpp"
#include "include/bitsort.hpp"

int next_power_of_two(int n) {
    return std::pow ( 2, std::ceil(std::log2(n)) );
}

int main() try
{
    std::size_t original_size = 0;
    std::cin >> original_size;
    if ( original_size <= 0 ) { throw std::runtime_error ( "Error: invalid size"); }

    std::vector<int> data = {};
    for ( std::size_t i = 0, element = 0; i < original_size; ++i ) {
        std::cin >> element;
        if ( !std::cin.good() ) { throw std::runtime_error ( "Error : invalid argumnet" ); }
        data.push_back ( element );
    }

    std::size_t new_size = next_power_of_two ( original_size );
    data.resize ( new_size, std::numeric_limits<int>::max() );

    //const OCL::OclPlatform platform = {};
    OCL::OclApp<int> app { "kernels/bitonic_sort.cl", "bitonicSort", data }; 
    BitonicSort::GpuBitonicSort<int> bsrt = { app, data  };
    
    bsrt.sort( data );

    for ( std::size_t i = 0; i < original_size; ++i ) std::cout << data[i] << " ";
    std::cout << std::endl;

    return 0;
}
catch(const std::exception& exceptions )
{
    std::cerr << exceptions.what() << '\n';
    return 1;
}
catch (...) {}