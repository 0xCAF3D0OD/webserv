#include "StatusCodes.hpp"
#include "setting.hpp"

#include "../../../inc/Response.hpp"
#include "../../../inc/cgi.hpp"

std::map<std::string, std::string>
request_POST(void)
{
	std::map<std::string, std::string> map;
	char							   array[] = "Accept: */*\n"
												 "Accept-Encoding: gzip, deflate, br\n"
												 "Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3\n"
												 "Connection: keep-alive\n"
												 "Content-Length: 2\n"
												 "Content-Type: multipart/form-data; boundary=\n"
												 "Cookie: wp-settings-time-1=1677339113\n"
												 "Host: localhost:8081\n"
												 "Method: POST\n"
												 "Origin: http://localhost:8081\n"
												 "Path: /index.html\n"
												 "Protocol: HTTP/1.1\n"
												 "Referer: http://localhost:8081/submitPage.html\n"
												 "Sec-Fetch-Dest: empty\n"
												 "Sec-Fetch-Mode: no-cors\n"
												 "Sec-Fetch-Site: none\n"
												 "Upgrade-Insecure-Requests:\n"
												 "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:107.0) Gecko/20100101 "
												 "Firefox/107.0\n";

	std::stringstream ss(array);
	std::string		  line;
	while (std::getline(ss, line))
	{
		size_t s_count = line.find(": ");
		if (s_count != std::string::npos)
		{
			std::string key = line.substr(0, s_count);
			std::string value = line.substr(s_count + 2);
			map.insert(std::make_pair(key, value));
		}
	}
	return (map);
}

int
test_cgi_simple_POST(void)
{
	std::string						   cgi_path = PATH_CGI_PHP;
	std::string						   cgi_file = "test/website/cgi/path_info.php";
	std::string						   output_cgi;
	std::map<std::string, std::string> map;
	std::string						   str;

	map = request_POST();
	if (map.empty())
	{
		std::cerr << "map is empty" << std::endl;
		return (1);
	}
	return (0);
}

int
test_cgi_POST(void)
{
	std::string						   cgi_path = PATH_CGI_PHP;
	std::string						   cgi_file = "test/website/cgi/input.php";
	std::string						   output_cgi;
	std::map<std::string, std::string> map;
	std::string						   str;

	map = request_POST();
	std::string cgi_query = get_key(map, "Query");
	CGI			cgi(cgi_path, cgi_file, cgi_query);

	if ((output_cgi = cgi.execution_cgi(map, cgi_file, "salut")).empty())
	{
		std::cout << "1.1 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	if (output_cgi.find("No input file specified.") != std::string::npos)
	{
		std::cerr << "1.2 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	std::cout << output_cgi << std::endl;
	return (0);
}
