#include "Socket.hpp"

Socket::Socket(int domain, unsigned short port, int type, int protocol, const json::Value &server_config)
	: _server_config(server_config), _request(server_config), _response(server_config)
{
	_address.sin_family = domain;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	create_socket(domain, type, protocol);
	set_socket_non_blocking();
	binding_socket();
	start_listening();
	_header_str = "";
	_body_str = "";
}

void
Socket::create_socket(int domain, int type, int protocol)
{
	_socket_id = socket(domain, type, protocol);
	test_socket(_socket_id, "create_socket() Fail!");
}

void
Socket::binding_socket()
{
	int res(bind(_socket_id, reinterpret_cast<struct sockaddr *>(&_address), sizeof(_address)));
	test_socket(res, "binding_socket() Fail!");
}

void
Socket::start_listening()
{
	int res = listen(_socket_id, LISTEN_BACKLOG);
	test_socket(res, "start_listening() Fail!");
}

void
Socket::test_socket(int item_to_test, const char *msg)
{
	if (item_to_test < 0)
	{
		close(_socket_id);
		throw std::runtime_error(msg);
	}
}

int
Socket::get_socket_id() const
{
	return _socket_id;
}

void
Socket::set_socket_non_blocking()
{
	int val = 1;
	int ret = setsockopt(_socket_id, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	test_socket(ret, "setsockopt() Fail!");

	ret = fcntl(_socket_id, F_SETFL, O_NONBLOCK);
	test_socket(ret, "fcnt() Fail!");
}

int
Socket::socket_recv()
{
	char		buffer[MAXLINE] = { 0 };
	ssize_t		byte_read;
	std::string tmp_buffer;
	byte_read = recv(this->_connection_fd, buffer, MAXLINE - 1, 0);
	if (byte_read == 0 || byte_read == -1)
	{
		return byte_read;
	}
	tmp_buffer = std::string(buffer);
	size_t header_body_delimiter = tmp_buffer.find("\r\n\r\n");
	_header_str += tmp_buffer.substr(0, header_body_delimiter);
	if (header_body_delimiter + 4 < tmp_buffer.size())
	{
		for (size_t i = header_body_delimiter + 4; i < static_cast<unsigned long>(byte_read); i++)
			_body_str.push_back(buffer[i]);
		if (LOG_SOCKET)
			std::cout << "body str:" << _body_str << std::endl;
	}
	int ret = _request.parse_buffer(_header_str);
	if (_request._request_map["Content-Type"].compare("multipart/form-data") == 0)
	{
		if (LOG_SOCKET)
			std::cout << "Content-type = multipart/form-data" << std::endl;
		multipart_handler();
		std::memset(buffer, 0, MAXLINE);
	}
	std::cout << "ret: " << ret << std::endl;
	if (_request.get_method().compare("DELETE") == 0 && !ret)
	{
		delete_handler();
	}
	if (_request.get_method().compare("POST") == 0 && !ret
		&& (_request._request_map["Content-Type"].compare("multipart/form-data") != 0))
	{
		_response.body_post_cgi = _body_str;
	}
	_response.load_http_request(_request);
	clean_request();
	send_response();
	return byte_read;
}

void
Socket::multipart_handler()
{
	ssize_t		  byte_read;
	char		  buffer[MAXLINE] = { 0 };
	char		 *end = NULL;
	unsigned long content_length = std::strtoul(_request._request_map["Content-Length"].c_str(), &end, 10);

	while (_body_str.size() < content_length)
	{
		std::memset(buffer, 0, MAXLINE);
		byte_read = recv(this->_connection_fd, buffer, MAXLINE - 1, 0);
		_body_str.append(buffer, byte_read);
	}
	if (LOG_SOCKET)
		std::cout << "_body_str.size(): " << _body_str.size() << std::endl;
	check_content_lenght_authorized();
	if (_request.get_error_code() == 0)
		create_new_file();
}

void
Socket::delete_handler()
{
	std::string file_name = _request.get_path();
	std::string path = _server_config.get("path").get<std::string>();
	std::string fullpath = path + file_name;
	std::cout << fullpath << std::endl;
	fullpath = utils::my_replace(fullpath, "%20", " ");
	if (access(fullpath.c_str(), F_OK) != -1)
	{
		_request._request_map["fileStatus"] = "exist";
		int ret = remove(fullpath.c_str());
		if (ret != 0)
			_request._request_map["fileStatus"] = "r_fail";
	}
}

int
Socket::socket_accept()
{
	_connection_fd = accept(get_socket_id(), NULL, NULL);
	return _connection_fd;
}

std::string
Socket::get_file_full_name()
{
	std::string start_looking = _body_str.substr(_body_str.find("filename="));
	start_looking = start_looking.substr(start_looking.find_first_of('"'), start_looking.find("\n"));
	size_t		position_quote_start(start_looking.find_first_of('"') + 1);
	size_t		length(start_looking.find_first_of('"', +1) - position_quote_start);
	std::string file_name = start_looking.substr(position_quote_start, length);
	std::string fullpath = _server_config.get("path").get<std::string>() + "/uploads/" + file_name;
	return utils::my_replace(fullpath, "%20", " ");
}

void
Socket::create_new_file()
{
	std::string fullpath = get_file_full_name();
	if (LOG_SOCKET)
		std::cout << "new file: \"" << fullpath << "\"" << std::endl;
	if (access(fullpath.c_str(), F_OK))
	{
		size_t		delimiter = _body_str.find("Content-Type");
		std::string file_part
			= _request.trim(_body_str.substr(_body_str.find_first_of("\r\n\r\n", delimiter)));
		if (LOG_SOCKET)
			std::cout << "file_part: " << file_part << std::endl;
		size_t		end = file_part.find(_request._request_map["boundary"]);
		std::string half_clean_file = file_part.substr(0, end);
		std::string clean_file = clean_end_of_file(half_clean_file);
		if (LOG_SOCKET)
			std::cout << "### clean file ###\n" << clean_file << "\n###/clean file ###\n" << std::endl;
		std::ofstream ofs(fullpath.c_str(), std::ios_base::out | std::ios_base::binary);
		ofs.write(clean_file.c_str(), clean_file.size() - 1);
		ofs.close();
	}
	else
	{
		_request._request_map["fileStatus"] = "exist";
	}
}

std::string
Socket::clean_end_of_file(std::string const &str_to_clean)
{
	int index = str_to_clean.size() - 1;
	while (str_to_clean[index] == '-' || str_to_clean[index] == '\n')
		--index;
	return str_to_clean.substr(0, index);
}

void
Socket::clean_request()
{
	_header_str = "";
	_body_str = "";
	_request._request_map.clear();
	_request.set_query_false();
	_request.set_error_code(0);
}

void
Socket::send_response()
{
	int			send_ret = 0;
	std::string full_response_str(this->_response.get_http_response());
	send_ret = send(_connection_fd, full_response_str.c_str(), full_response_str.length(), 0);
	if (send_ret < static_cast<int>(full_response_str.length()))
	{
		send_ret = send(_connection_fd, full_response_str.c_str(), full_response_str.length(), 0);
	}
	close(_connection_fd);
}

void
Socket::check_content_lenght_authorized()
{
	http::Request::t_object req_map = _request.get_map();
	if (_server_config.if_exist("max_length") && (req_map.find("Content-Length") != req_map.end()))
	{
		char *end = NULL;
		if (_server_config.get("max_length").get<double>()
			< std::strtoul(req_map["Content-Length"].c_str(), &end, 10))
		{
			_request.set_error_code(413);
		}
	}
}
