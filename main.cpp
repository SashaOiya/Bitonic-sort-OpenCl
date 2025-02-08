#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 200

#include <CL/opencl.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits>

// Функция чтения OpenCL-ядра из файла
std::string readKernelFile(const std::string& filename) {
    std::ifstream file(filename);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Функция вывода массива
void printArray(const std::vector<int>& arr, int size) {
    for (int i = 0; i < size; i++) std::cout << arr[i] << " ";
    std::cout << std::endl;
}

// Функция округления размера массива до ближайшей степени двойки
int nextPowerOfTwo(int n) {
    return std::pow(2, std::ceil(std::log2(n)));
}

int main() {

    int originalSize = 0;
    std::cin >> originalSize;
    int element = 0;
    std::vector<int> data = {};
    for ( int i = 0; i < originalSize; ++i ) {
        std::cin>>element;
        // error
        data.push_back ( element );
    }
    int N = nextPowerOfTwo(originalSize); // Округляем до степени двойки

    // Дополняем массив элементами INT_MAX (чтобы они не мешали сортировке)
    data.resize(N, std::numeric_limits<int>::max());

    std::cout << "Исходный массив:\n";
    printArray(data, originalSize);

    // 1. Инициализация OpenCL
    cl::Platform platform = cl::Platform::getDefault();
    cl::Device device = cl::Device::getDefault();
    cl::Context context({ device });
    cl::CommandQueue queue(context, device, CL_QUEUE_PROFILING_ENABLE);

    // 2. Компиляция OpenCL-ядра
    std::string kernelCode = readKernelFile("kernels/bitonic_sort.cl");
    cl::Program program(context, kernelCode);
    program.build({ device });

    // 3. Создаём буфер OpenCL
    cl::Buffer bufferData(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * N, data.data());

    // 4. Запуск OpenCL-ядра
    cl::Kernel kernel(program, "bitonic_sort");
    kernel.setArg(0, bufferData);
    kernel.setArg(1, N);

        // Устанавливаем аргументы ОДИН РАЗ
    kernel.setArg(0, bufferData);
    kernel.setArg(1, N);

    // Запускаем ядро **один раз** вместо цикла
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(N), cl::NullRange);

    // Ожидаем завершения (только один раз!)
    queue.finish();

    // 5. Чтение отсортированного массива
    queue.enqueueReadBuffer(bufferData, CL_TRUE, 0, sizeof(int) * N, data.data());

    std::cout << "Отсортированный массив:\n";
    printArray(data, originalSize); // Выводим только реальные элементы

    return 0;
}

