#include "Request.hpp"

namespace http
{

Request::Request(const json::Value &server_config)
	: _server_config(server_config), _error_code(0), _has_query(false)
{
}
Request::~Request() {}

int
Request::parse_buffer(std::string str_buff)
{
	std::vector<std::string> tmp_vector;
	std::string				 delimiter = "\r\n";
	int						 delimiter_position = str_buff.find(delimiter);
	while (delimiter_position != -1)
	{
		std::string str = str_buff.substr(0, delimiter_position);
		tmp_vector.push_back(str);
		str_buff.erase(str_buff.begin(), str_buff.begin() + delimiter_position + 1);
		delimiter_position = str_buff.find(delimiter);
	}
	this->parse_first_line(tmp_vector[0]);
	this->parse_other_lines(tmp_vector);
	check_header();
	check_redirection();
	int ret = check_path_and_method();
	empty_path_handler();
	return ret;
}

void
Request::parse_first_line(std::string firstLine)
{
	std::vector<std::string> tmp_vector;
	std::string				 delimiter = " ";
	int						 delimiter_position = firstLine.find(delimiter);
	while (delimiter_position != -1)
	{
		std::string str = firstLine.substr(0, delimiter_position);
		tmp_vector.push_back(this->trim(str));
		firstLine.erase(firstLine.begin(), firstLine.begin() + delimiter_position + 1);
		delimiter_position = firstLine.find(delimiter);
	}
	tmp_vector.push_back(this->trim(firstLine));

	if (tmp_vector.size() != 3)
	{
		set_error_code(400);
		return;
	}
	_request_map["Method"] = tmp_vector[0];
	_request_map["Path"] = tmp_vector[1];
	_request_map["Protocol"] = tmp_vector[2];
}

void
Request::parse_other_lines(std::vector<std::string> &tmp_vector)
{
	std::string			   delimiter = ":";
	std::string			   key;
	std::string			   value;
	std::string::size_type delimiter_position;
	for (size_t i = 1; i < tmp_vector.size(); i++)
	{
		delimiter_position = tmp_vector[i].find(delimiter);
		key = tmp_vector[i].substr(0, delimiter_position);
		value = tmp_vector[i].substr(delimiter_position + 1);
		if (Request::trim(key).length() != 0 || Request::trim(value).length() != 0)
			_request_map[Request::trim(key)] = Request::trim(value);
	}
}

std::string
Request::trim(const std::string &s)
{
	const std::string _WHITESPACE = " \n\r\t\f\v";
	std::string		  left_trimed_string = "";
	size_t			  start = s.find_first_not_of(_WHITESPACE);
	if (start == std::string::npos)
	{
		return left_trimed_string;
	}
	else
	{
		left_trimed_string = s.substr(start);
	}
	size_t end = left_trimed_string.find_last_not_of(_WHITESPACE);
	return left_trimed_string.substr(0, end + 1);
}

std::string
Request::get_method() const
{
	if (_request_map.find("Method") != _request_map.end())
		return _request_map.at("Method");
	return "";
}

std::string
Request::get_path() const
{
	if (_request_map.find("Path") != _request_map.end())
		return _request_map.at("Path");
	return "";
}

std::string
Request::get_protocol() const
{
	if (_request_map.find("Protocol") != _request_map.end())
		return _request_map.at("Protocol");
	return "";
}

std::string
Request::get_host() const
{
	if (_request_map.find("Host") != _request_map.end())
		return _request_map.at("Host");
	return "";
}

std::ostream &
operator<<(std::ostream &output, Request const &req)
{
	for (Request::t_object::const_iterator start = req.get_map().begin(); start != req.get_map().end();
		 ++start)
	{
		output << start->first << " : " << start->second << "\n";
	}
	return output;
}

const Request::t_object &
Request::get_map() const
{
	return _request_map;
}

void
Request::clean_content_type()
{
	size_t end_of_first_part = _request_map["Content-Type"].find_first_of(";");
	if (_request_map["Content-Type"].find("boundary=") != std::string::npos)
	{
		_request_map["boundary"]
			= _request_map["Content-Type"].substr(_request_map["Content-Type"].find("boundary=") + 9);
	}
	_request_map["Content-Type"] = _request_map["Content-Type"].substr(0, end_of_first_part);
}

void
Request::check_if_has_query()
{
	size_t		last_slash = _request_map["Path"].find_last_of('/');
	std::string last_part = _request_map["Path"].substr(last_slash + 1);
	size_t		question_mark = last_part.find_first_of('?');
	if (question_mark == std::string::npos)
		return;
	_has_query = true;
	_request_map["Query"] = last_part.substr(question_mark + 1);
}

bool
Request::get_has_query() const
{
	return _has_query;
}

void
Request::set_query_false()
{
	_has_query = false;
}

void
Request::clean_path()
{
	size_t question_mark = _request_map["Path"].find_first_of('?');
	_request_map["Path"] = _request_map["Path"].substr(0, question_mark);
}

void
Request::empty_path_handler()
{
	if (_request_map["Path"].size() == 1 && _request_map["Path"][0] == '/')
	{
		_request_map["Path"] = "/index.html";
	}
}

void
Request::check_header()
{
	if (_request_map.find("Content-Type") != _request_map.end())
		this->clean_content_type();
	check_if_has_query();
	if (_has_query)
		clean_path();
}

int
Request::get_error_code() const
{
	return _error_code;
}
void
Request::set_error_code(int code)
{
	_error_code = code;
}

int
Request::check_path_and_method()
{
	std::string path = _server_config.get("path").get<std::string>();
	path += get_path();
	path = utils::my_replace(path, "%20", " ");
	if (access(path.c_str(), F_OK))
		return 0;
	if (!_server_config.if_exist("method_allowed"))
		return 0;
	Method method(_server_config.get("method_allowed").get<json::t_object>());
	if (!method.is_allowed(get_path(), get_method()))
	{
		if (get_error_code() == 0)
		{
			set_error_code(405);
		}
		return 1;
	}
	return 0;
}

void
Request::check_redirection()
{
	if (_server_config.if_exist("redirection"))
	{
		Redirection redirection(_server_config.get("redirection").get<json::t_object>());
		std::string new_url = "";
		if (redirection.is_redirection(get_path(), new_url))
		{
			_request_map["Location"] = new_url;
			if (get_error_code() == 0)
				set_error_code(301);
		}
	}
}

} /* namespace http */
