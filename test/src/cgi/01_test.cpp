#include <string>

std::string execve_cgi(char **arguments);

int
test_execve_cgi(void)
{
	char	   *arguments[3];
	std::string output;

	arguments[0] = (char *)"/bin/echo";
	arguments[1] = (char *)"salut";
	arguments[2] = 0;

	output = execve_cgi(arguments);

	if (output != "salut\n")
		return 1;

	return 0;
}
