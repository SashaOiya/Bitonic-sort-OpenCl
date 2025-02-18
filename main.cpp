#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits>
#include <bit>

//#include "include/ocl.hpp"
#include "include/bitsort.hpp"

int main() try
{
    std::size_t original_size = 0;
    std::cin >> original_size;
    if ( original_size <= 0 ) { throw std::runtime_error ( "Error: invalid size"); }

    std::vector<int> data = {};
    for ( int i = 0, element = 0; i < original_size; ++i ) {
        std::cin >> element;
        if ( !std::cin.good() ) { throw std::runtime_error ( "Error : invalid argumnet" ); }
        data.push_back ( element );
    }

    std::size_t new_size = std::bit_ceil ( original_size );
    data.resize ( new_size, std::numeric_limits<int>::max() );

    //const OCL::OclPlatform platform = {};
    OCL::OclApp<int> app { "kernels/bitonic_sort.cl", "bitonicSort", data }; 
    BitonicSort::GpuBitonicSort<int> bsrt = { app, data  };
    
    bsrt.sort( data );

    for ( std::size_t i = 0; i < original_size; ++i ) std::cout << data[i] << " ";
    std::cout << std::endl;

    return 0;
}
catch(const std::exception& exception )
{
    std::cerr << exception.what() << '\n';
    return 1;
}
catch (...) 
{
    std::cerr << "Caught unknown exception\n";
    return 1;
}