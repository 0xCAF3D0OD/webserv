#ifndef WEBSERV_CGI_HPP
#define WEBSERV_CGI_HPP

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sys/stat.h>
#include <sys/wait.h>
#include <typeinfo>
#include <unistd.h>
#include <vector>

#define BUFFER_SIZE 4092

void print_container_map(std::map<std::string, std::string> myMap);

std::string get_key(const std::map<std::string, std::string> &map, const std::string &key);

class CGI
{
  private:
	std::map<std::string, std::string> _env;
	std::vector<char *>				   _args;
	std::string						   _output_cgi;
	int								   _p_out[2];
	int								   _p_in[2];
	char							   _read_buffer[BUFFER_SIZE];

	std::string _cont_type;
	std::string _cont_length;
	std::string _location;
	std::string _env_Host;
	std::string _http_version;
	std::string _port;
	std::string _method;
	std::string _script_path;
	std::string _script_name;
	std::string _query;

  public:
	CGI(void);
	CGI(const std::string &bin, const std::string &file, const std::string &query);

	std::string execution_cgi(const std::map<std::string, std::string> &map, const std::string &args,
							  const std::string &body_post_cgi);
	std::string parent_process(pid_t &pid, const std::string &get_body_cgi);
	void		child_process(char **);
	void		set_env(const std::map<std::string, std::string> &map, const std::string &args);
	void		check_map(const std::map<std::string, std::string> &map, const std::string &name_file);
	~CGI(void);
};

#endif // WEBSERV_CGI_HPP
