#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "StatusCodes.hpp"
#include "Value.hpp"
#include "cgi.hpp"
#include "std98.hpp"
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

namespace http
{

class Response
{
  public:
	typedef std::map<std::string, std::string> t_object;

	Response(const json::Value &server_config);
	~Response(void);
	std::string		get_http_response(void);
	void			load_http_request(Request &req);
	const t_object &get_map() const;
	std::string		body_post_cgi;

  private:
	const json::Value &_server_config;
	static StatusCode  _status_code;
	t_object		   _response_map;
	void			   set_content_length(const std::string &str);
	void			   init_response_map(void);
	void			   set_response_type(std::string path);
	void			   load_response_with_path(int status_code, const std::string &path);
	void			   load_response_without_path(int status_code);

	std::string get_time_stamp(void);
	void		construct_header_string(void);
	void		construct_body_string(std::string path_to_file);
	void		construct_full_response(void);
	void		create_error_html_page(int code);
	bool		has_php_extension(const Request &request) const;
	void		php_handler(const Request &request);
	bool		check_if_is_dir(const std::string &path);
	void		handle_request_with_error(Request &request);
	void		fill_header_firstpart(int status_code);
	void		fill_header_lastpart();
	void		handle_response_with_status_code(int status_code);
	void		load_response_cgi(int status_code);
};

std::ostream &operator<<(std::ostream &, Response const &);

} /* namespace http */

#endif
