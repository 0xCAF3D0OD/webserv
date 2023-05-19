#include "StatusCodes.hpp"
#include "setting.hpp"

#include "../../../inc/Response.hpp"
#include "../../../inc/cgi.hpp"

std::map<std::string, std::string>
request_GET(void)
{
	std::map<std::string, std::string> map;
	char							   array[]
		= "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\n"
		  "Accept-Encoding: gzip, deflate, br\n"
		  "Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;\n"
		  "q=0.3 Connection: keep-alive\n"
		  "Content-Type:\n"
		  "Cookie: wp-settings-time-1=1677339113\n"
		  "Host: localhost:6060\n"
		  "Method: GET\n"
		  "Path: /input.php\n"
		  "Protocol: HTTP/1.1\n"
		  "Query: name=kevin\n"
		  "Sec-Fetch-Dest: document\n"
		  "Sec-Fetch-Mode: navigate\n"
		  "Sec-Fetch-Site: none\n"
		  "Upgrade-Insecure-Requests: 1\n"
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
test_php_output(void)
{
	std::map<std::string, std::string> map;
	std::string						   str;
	CGI								   cgi;

	map = request_GET();
	if (map.empty())
	{
		std::cerr << "map is empty" << std::endl;
		return (1);
	}
	return (0);
}

int
test_cgi_with_php(void)
{
	std::string						   cgi_path = PATH_CGI_PHP;
	std::string						   cgi_file = "test/website/cgi/input.php";
	std::map<std::string, std::string> map;
	std::string						   output_cgi;

	map = request_GET();
	std::map<std::string, std::string>::iterator it;
	CGI											 cgi(cgi_path, cgi_file, "");

	if ((output_cgi = cgi.execution_cgi(map, cgi_file, "salut")).empty())
	{
		std::cerr << "2.1 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	return (0);
}

int
test_cgi_with_php_query(void)
{
	std::string						   cgi_path = PATH_CGI_PHP;
	std::string						   cgi_file = "test/website/cgi/input.php";
	std::map<std::string, std::string> map;
	std::string						   output_cgi;

	map = request_GET();
	std::map<std::string, std::string>::iterator it;
	CGI											 cgi(cgi_path, cgi_file, "");

	if ((output_cgi = cgi.execution_cgi(map, cgi_file, "salut")).empty())
	{
		std::cerr << "3.1 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	if (output_cgi.find("No input file specified.") != std::string::npos)
	{
		std::cerr << "3.2 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	return (0);
}

int
test_cgi_with_php_query2(void)
{
	std::string						   cgi_path = PATH_CGI_PHP;
	std::string						   cgi_file = "test/website/cgi/path_info.php";
	std::map<std::string, std::string> map;
	std::string						   output_cgi;

	map = request_GET();
	std::string									 cgi_query = get_key(map, "Query");
	std::map<std::string, std::string>::iterator it;
	CGI											 cgi(cgi_path, cgi_file, cgi_query);

	if ((output_cgi = cgi.execution_cgi(map, cgi_file, "salut")).empty())
	{
		std::cerr << "4.1 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	if (output_cgi.find("No input file specified.") != std::string::npos)
	{
		std::cerr << "4.2 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	return (0);
}

int
test_cgi_with_files(void)
{
	std::string						   cgi_path = PATH_CGI_PHP;
	std::string						   cgi_file = "test/website/cgi/path_info.php";
	std::map<std::string, std::string> map;
	std::string						   output_cgi;

	map = request_GET();
	std::string									 cgi_query = get_key(map, "Query");
	std::map<std::string, std::string>::iterator it;
	CGI											 cgi(cgi_path, cgi_file, cgi_query);

	if ((output_cgi = cgi.execution_cgi(map, cgi_file, "salut")).empty())
	{
		std::cerr << "5.1.1 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	if (output_cgi.find("No input file specified.") != std::string::npos)
	{
		std::cerr << "5.1.2 Error: output: " << output_cgi << std::endl;
		return (1);
	}

	cgi_file = "test/website/cgi/input.php";

	CGI cgi1(cgi_path, cgi_file, cgi_query);

	if ((output_cgi = cgi1.execution_cgi(map, cgi_file, "salut")).empty())
	{
		std::cerr << "5.2.1 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	if (output_cgi.find("No input file specified.") != std::string::npos)
	{
		std::cerr << "5.2.2 Error: output: " << output_cgi << std::endl;
		return (1);
	}

	cgi_file = "test/website/cgi/result.php";

	CGI cgi2(cgi_path, cgi_file, cgi_query);

	if ((output_cgi = cgi2.execution_cgi(map, cgi_file, "salut")).empty())
	{
		std::cerr << "5.3.1 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	if (output_cgi.find("No input file specified.") != std::string::npos)
	{
		std::cerr << "5.3.2 Error: output: " << output_cgi << std::endl;
		return (1);
	}

	cgi_file = "test/website/cgi/post.html";

	CGI cgi3(cgi_path, cgi_file, cgi_query);

	if ((output_cgi = cgi3.execution_cgi(map, cgi_file, "salut")).empty())
	{
		std::cerr << "5.4.1 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	if (output_cgi.find("No input file specified.") != std::string::npos)
	{
		std::cerr << "5.4.2 Error: output: " << output_cgi << std::endl;
		return (1);
	}

	cgi_file = "test/website/cgi/status.php";

	CGI cgi4(cgi_path, cgi_file, cgi_query);

	if ((output_cgi = cgi4.execution_cgi(map, cgi_file, "salut")).empty())
	{
		std::cerr << "5.5.1 Error: output: " << output_cgi << std::endl;
		return (1);
	}
	if (output_cgi.find("No input file specified.") != std::string::npos)
	{
		std::cerr << "5.5.2 Error: output: " << output_cgi << std::endl;
		return (1);
	}

	return (0);
}
