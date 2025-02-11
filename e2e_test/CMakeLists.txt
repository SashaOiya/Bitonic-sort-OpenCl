add_executable(ocl_test
			   tests.cpp)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../")

set_target_properties(ocl_test PROPERTIES
	CXX_STANDART 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

target_compile_definitions(ocl_test PUBLIC
						   -DTEST_DATA_DIR="${TEST_DATA_DIR}")

target_link_libraries(ocl_test PRIVATE
					  GTest::GTest
					  GTest::gtest_main)
target_link_libraries(ocl_test PRIVATE ${OpenCL_LIBRARIES})

target_include_directories(ocl_test PUBLIC
	                       ${PROJECT_SOURCE_DIR}/include
						   ${PROJECT_SOURCE_DIR}/e2e_test)

add_definitions(-DCL_HPP_TARGET_OPENCL_VERSION=300)

include(GoogleTest)
gtest_discover_tests(ocl_test)