#include "Response.hpp"

namespace http
{

StatusCode Response::_status_code;

Response::Response(const json::Value &server_config) : _server_config(server_config) { body_post_cgi = ""; }

Response::~Response(void) {}

void
Response::load_http_request(Request &request)
{
	init_response_map();
	if (request.get_error_code() != 0)
	{
		handle_request_with_error(request);
		return;
	}
	std::string path = _server_config.get("path").get<std::string>();
	path += request.get_path();
	if (has_php_extension(request) && (request.get_method().compare("POST") != 0))
	{
		if (access(path.c_str(), F_OK))
			load_response_with_path(404, path);
		else
			php_handler(request);
		return;
	}
	init_response_map();

	if (request.get_method().compare("GET") == 0)
	{
		if (access(path.c_str(), F_OK))
			load_response_with_path(404, path);
		else
			load_response_with_path(200, path);
	}
	else if (request.get_method().compare("POST") == 0)
	{
		if (request._request_map["fileStatus"].compare("exist") == 0)
			load_response_without_path(409);
		else
			load_response_without_path(201);
	}
	else if (request.get_method().compare("DELETE") == 0)
	{
		if (request._request_map["fileStatus"].compare("exist") == 0)
			load_response_without_path(204);
		else if (request._request_map["fileStatus"].compare("r_fail") == 0)
			load_response_without_path(500);
		else
			load_response_without_path(404);
	}
	else
		load_response_with_path(405, path);
}

void
Response::init_response_map()
{
	_response_map["Status-line"] = "";
	_response_map["Date"] = "";
	_response_map["Server"] = _server_config.get("server_name").get<std::string>();
	_response_map["Content-Length"] = "";
	_response_map["Content-Type"] = "";
	_response_map["Connection"] = "Closed";
	_response_map["Protocol"] = "HTTP/1.1 ";
	_response_map["header-string"] = "";
	_response_map["body-string"] = "";
	_response_map["full-response-string"] = "";
}

void
Response::load_response_with_path(int status_code, const std::string &path)
{
	if (check_if_is_dir(path))
		status_code = 401;
	fill_header_firstpart(status_code);
	if (status_code != 200)
		handle_response_with_status_code(status_code);
	else
	{
		set_response_type(path);
		construct_body_string(path);
	}
	fill_header_lastpart();
}

void
Response::load_response_without_path(int status_code)
{
	fill_header_firstpart(status_code);
	fill_header_lastpart();
}

std::string
Response::get_time_stamp(void)
{
	std::time_t stamp = std::time(NULL);
	std::string formated_date = std::asctime(std::gmtime(&stamp));
	formated_date = formated_date.substr(0, formated_date.length() - 1);
	formated_date += " GMT";
	return formated_date;
}

void
Response::set_content_length(std::string str)
{
	_response_map["Content-Length"] = std98::to_string(str.length());
}

void
Response::set_response_type(std::string path)
{
	std::string type = "";
	type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));
	if (type == "html")
		_response_map["Content-Type"] = "text/html";
	else if (type == "css")
		_response_map["Content-Type"] = "text/css";
	else if (type == "js")
		_response_map["Content-Type"] = "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		_response_map["Content-Type"] = "image/jpeg";
	else if (type == "png")
		_response_map["Content-Type"] = "image/png";
	else if (type == "bmp")
		_response_map["Content-Type"] = "image/bmp";
	else if (type == "ico")
		_response_map["Content-Type"] = "image/x-icon";
	else if (type == "json")
		_response_map["Content-Type"] = "application/json";
	else
		_response_map["Content-Type"] = "text/plain";
}

void
Response::construct_header_string(void)
{
	std::string CRLF = "\r\n";

	_response_map["header-string"] = _response_map["Status-line"] + CRLF + "Date: " + _response_map["Date"]
									 + CRLF + "Server: " + _response_map["Server"] + CRLF
									 + "Content-Length: " + _response_map["Content-Length"] + CRLF
									 + "Content-Type: " + _response_map["Content-Type"] + CRLF
									 + "Connection: " + _response_map["Connection"] + CRLF + CRLF;
}

void
Response::construct_body_string(std::string path_to_file)
{
	std::ifstream	  file;
	std::stringstream buffer;

	file.open(path_to_file.c_str());
	if (file.fail())
	{
		std::cerr << "Open file error" << std::endl;
		return;
	}
	buffer << file.rdbuf();
	_response_map["body-string"] = buffer.str();
	file.close();
}

void
Response::construct_full_response(void)
{
	_response_map["full-response-string"] = _response_map["header-string"] + _response_map["body-string"];
}

std::string
Response::get_http_response(void)
{
	return _response_map["full-response-string"];
}

void
Response::create_error_html_page(int code)
{
	_response_map["body-string"]
		= "<!DOCTYPE html><html><head><link rel=\"stylesheet\"href=\"style.css\"/><link rel=\"icon\" "
		  "href=\"favicon.ico\"><title>"
		  + std98::to_string(code)
		  + "</title></head><body><div class=\" wrapper\"><div class=\"centered-box\"><h1 class=\"title\">"
		  + _status_code.get_key_value_formated(code) + "</h1></div></div></body></html>";
}

std::ostream &
operator<<(std::ostream &output, Response const &response)
{
	for (Response::t_object::const_iterator start(response.get_map().begin());
		 start != response.get_map().end(); ++start)
	{
		output << start->first << " : " << start->second << "\n";
	}
	return output;
}

const Response::t_object &
Response::get_map() const
{
	return _response_map;
}

bool
Response::has_php_extension(const Request &request) const
{
	std::string path(request.get_path());
	size_t		last_dot = path.find_last_of('.');
	if (last_dot != std::string::npos)
	{
		std::string extenstion(path.substr(last_dot));
		return (extenstion.compare(".php") == 0);
	}
	return false;
}

void
Response::php_handler(const Request &request) const
{
	t_object req_map = request.get_map();
	std::cout << "IT's a .php" << std::endl;
	std::cout << body_post_cgi << std::endl;


//	std::string cgi_path = _server_config.get("php-cgi").get<std::string>();
	std::string cgi_path = "/Users/kdi-noce/goinfre/php/php-8.2.5/sapi/cgi/php-cgi";
	if (request.get_has_query())
		std::cout << "the query string is : " << req_map["Query"] << std::endl;

	std::string cgi_file = "test/website" + get_key(req_map, "Path");
	std::string cgi_query = get_key(req_map, "Query");
	std::string output_cgi;
	CGI			cgi(cgi_path, cgi_file, cgi_query);

	output_cgi = cgi.execution_cgi(req_map, cgi_file);
	size_t pos;
	if ((pos = output_cgi.find('<')) == std::string::npos)
		std::cerr << "wrong format file" << std::endl;
	else
	{
		output_cgi = output_cgi.substr(pos);
		std::cout << "output = \n" << output_cgi << std::endl;
	}
}

bool
Response::check_if_is_dir(const std::string &path)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
	{
		return false;
	}
	return S_ISDIR(info.st_mode);
}

void
Response::handle_request_with_error(Request &request)
{
	if (_server_config.if_exist(std98::to_string(request.get_error_code())))
	{
		int			status_code = request.get_error_code();
		std::string file_name = std98::to_string(status_code) + ".html";
		std::string fullpath = _server_config.get("path").get<std::string>() + "/" + file_name;
		fill_header_firstpart(status_code);
		if (access(fullpath.c_str(), F_OK))
			create_error_html_page(status_code);
		else
			construct_body_string(fullpath);
		fill_header_lastpart();
		return;
	}
	load_response_without_path(request.get_error_code());
	request.set_error_code(0);
	return;
}

void
Response::fill_header_firstpart(int status_code)
{
	_response_map["Date"] = get_time_stamp();
	_response_map["Status-line"]
		= _response_map["Protocol"] + _status_code.get_key_value_formated(status_code);
}

void
Response::fill_header_lastpart()
{
	set_content_length(_response_map["body-string"]);
	construct_header_string();
	construct_full_response();
}

void
Response::handle_response_with_status_code(int status_code)
{
	set_response_type("html");
	if (_server_config.if_exist("dir_error") && status_code == 401)
	{
		std::string file_path = _server_config.get("path").get<std::string>() + "/"
								+ _server_config.get("dir_error").get<std::string>();
		if (access(file_path.c_str(), F_OK))
			create_error_html_page(status_code);
		else
			construct_body_string(file_path);
	}
	else
		create_error_html_page(status_code);
}

} /* namespace http */
