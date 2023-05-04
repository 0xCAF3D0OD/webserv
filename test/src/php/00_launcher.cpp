#include "Unit_test.hpp"
#include "tests.hpp"

int
launch_php(void)
{
	Unit_test list("php");

	list.load_test("check_webs_output_php", &test_php_output);
	list.load_test("check_cgi_php", &test_cgi_with_php);
	return (list.launch_tests());
}