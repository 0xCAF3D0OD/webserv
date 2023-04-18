#include "Unit_test.hpp"
#include "tests.hpp"

int
execve_cgi_launcher(void)
{
	Unit_test list("execve cgi");

	list.load_test("simple test", &test_execve_cgi);
	return (list.launch_tests());
}
