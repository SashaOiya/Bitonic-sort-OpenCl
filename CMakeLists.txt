cmake_minimum_required(VERSION 3.11)

project(ocl)

set ( CXX_STANDARD 20 )
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(GTest REQUIRED)
find_package(OpenCL REQUIRED)
find_package(benchmark REQUIRED)
enable_testing()
add_subdirectory(e2e_test)

add_executable(ocl ./main.cpp)

target_link_libraries(ocl PRIVATE ${OpenCL_LIBRARIES})
