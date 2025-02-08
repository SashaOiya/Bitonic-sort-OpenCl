#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <CL/opencl.hpp>

#include <gtest/gtest.h>

const char* kernel_code = R"(
        __kernel void bitonic_sort(__global int* data, int N, int k, int j) {
        int tid = get_global_id(0);  // Индекс текущего потока

        // Индекс для сравнения
        int ixj = tid ^ j;

        // Проверяем, что индекс в пределах массива
        if (ixj > tid && ixj < N) {
            // Определяем направление сортировки
            bool ascending = ((tid & k) == 0); 

            int A = data[tid];
            int B = data[ixj];

            // Выполняем сортировку, если нужно
            if ((ascending && A > B) || (!ascending && A < B)) {
                data[tid] = B;
                data[ixj] = A;
            }
        }
    }
)";

namespace test_funcs
{
    using KeyT = int;

    size_t nextPowerOfTwo ( const size_t n )
    {
        return std::pow(2, std::ceil(std::log2(n)));
    }

    // Функция чтения OpenCL-ядра из файла
    std::string readKernelFile(const std::string& filename) {
        std::ifstream file(filename);
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    int compare(const void* a, const void* b) {
        int int_a = *(int*)a;
        int int_b = *(int*)b;
        
        // Возвращаем разницу, чтобы qsort мог отсортировать элементы
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

        int original_size = 0;
        file >> original_size;
        for ( int i = 0, element = 0; i < original_size; ++i ) {
            file >> element;
            // error
            res.push_back ( element );
        }
        const size_t new_data_size = nextPowerOfTwo (original_size) ; 
        res.resize ( new_data_size, std::numeric_limits<KeyT>::max() );

        cl::Platform platform = cl::Platform::getDefault();
        cl::Device device = cl::Device::getDefault();
        cl::Context context({ device });
        cl::CommandQueue queue(context, device, CL_QUEUE_PROFILING_ENABLE);
        
        //std::string kernelCode = readKernelFile ( kernelpath );
        cl::Program program(context, kernel_code);
        program.build ( { device } );
        cl::Buffer bufferData(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * new_data_size, res.data());
        cl::Kernel kernel(program, "bitonic_sort");
        kernel.setArg(0, bufferData); // передаём первый аргумент
        kernel.setArg(1, new_data_size ); // передаём второй аргумент

        // Запуск каждого шага битонической сортировки
        for (int k = 2; k <= new_data_size; k *= 2) {
            for (int j = k / 2; j > 0; j /= 2) {
                kernel.setArg(2, k);
                kernel.setArg(3, j);
                queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(new_data_size) );
                queue.finish();  // Важно: ждём завершения, чтобы гарантировать корректную работу
            }
        }

        queue.enqueueReadBuffer(bufferData, CL_TRUE, 0, sizeof(int) * original_size, res.data());
    }

    void get_answer ( const std::string& filename, std::vector<KeyT>& ans )
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
            // error
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

        std::vector<KeyT> res;
		get_result(test_path, res, kernel_path );

        std::vector<KeyT> ans;
		get_answer(test_path, ans);

        EXPECT_TRUE(res.size() == ans.size());
        for (int i = 0; i < ans.size(); i++)
        {
            EXPECT_EQ(res[i], ans[i]);
        }
	}
}