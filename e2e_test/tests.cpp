#include <string>

#include <gtest/gtest.h>
#include <CL/opencl.hpp>

#include "funcs.hpp"

/*TEST(test1, e2e)
{
	test_funcs::run_test("/test1.txt");
}*/

TEST(test2, e2e)
{
	test_funcs::run_test("/test2.txt");
}

/*TEST(test3, e2e)
{
	test_funcs::run_test("/test3.txt");
}

TEST(test4, e2e)
{
	test_funcs::run_test("/test4.txt");
}*/