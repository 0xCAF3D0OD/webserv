#include "Test.hpp"
#include <stdlib.h>

Test::Test(std::string const &name, int (*function_ptr)(void)) : _name(name), _function_ptr(function_ptr) {}

void
Test::do_test(void)
{
	int res;

	res = _function_ptr();
	_result.set_result(res);
}
