#pragma once

#include "ocl.hpp"

namespace BitonicSort {

template <typename T>
class GpuBitonicSort final {
    OCL::OclApp<T> app_;
    std::vector<T> data_;
    std::size_t size_ = 0;

    void bitonic_sort ( std::vector<T> &res )
    {
        auto err = app_.kernel().setArg(0, app_.buffer());
        if ( err != CL_SUCCESS ) { throw std::runtime_error ( "Error in kernel with setArg(0)"); }

        for ( int stage = 2; stage <= size_; stage *= 2) {
            for (int step = stage / 2; step > 0; step /= 2) {
                app_.kernel().setArg(1, stage);
                app_.kernel().setArg(2, step);
                err = app_.queue().enqueueNDRangeKernel(app_.kernel(), cl::NullRange,  cl::NDRange(size_), cl::NullRange);
                if ( err != CL_SUCCESS ) { throw std::runtime_error ( "Error in kernel with enqueueNDRangeKernel"); }
                app_.queue().finish();
            }
        }

        if ( res.size() < size_ ) { res.resize ( size_, 0 ); }
        err = app_.queue().enqueueReadBuffer(app_.buffer(), CL_TRUE, 0, sizeof(T) * size_, res.data());
        if ( err != CL_SUCCESS ) { throw std::runtime_error ( "Error in kernel with enqueueReadBuffer" ); }
    }

public: 
    GpuBitonicSort ( OCL::OclApp<T> &app, std::vector<T> &data) : app_(app), data_(data), size_(data_.size()) {} 

    void sort ( std::vector<T> &res ) { bitonic_sort(res); }
};

}