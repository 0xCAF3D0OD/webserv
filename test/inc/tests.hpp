#ifndef TESTS_H
#define TESTS_H

/* int check_argc_number(int); */

int launch_arguments(void);
int test_check_argc_number_1(void);
int test_check_argc_number_2(void);

/* arguments::check_argv_access */

int launch_arguments_argv(void);
int test_check_argv_access_valid(void);
int test_check_argv_access_unvalid(void);

/* http::Request */

int launch_http_request(void);
int test_http_request(void);

/* http::Response */

int launch_http_response(void);
int test_http_response(void);

/* http::StatusCodes */

int launch_status_code(void);
int test_status_code(void);

/* Json::read(char const *path); */

int launch_json(void);
int test_json_normal_file(void);
int test_json_bad_path(void);
int test_json_empty_string(void);
int test_json_missing_brace(void);
int test_json_number(void);
int test_json_string_number(void);
int test_json_object00(void);
int test_json_object01(void);

/* json::Value */

int launch_value(void);
int test_value_string(void);
int test_value_number(void);
int test_value_array_number(void);
int test_value_boolean(void);
int test_value_map_value(void);
int test_value_object_array(void);
int test_value_no_existing_key(void);

/* json::File */

int launch_file(void);
int test_file(void);
int test_file_all_file(void);
int test_file_not_valid_file(void);
int test_file_get_string(void);
int test_file_get_number(void);
int test_file_get_string_empty_string(void);

/* execve cgi */

int execve_cgi_launcher(void);
int test_execve_cgi(void);

#endif /* TESTS_H */
