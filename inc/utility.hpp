#ifndef WEBSERV_UTILITY_HPP
#define WEBSERV_UTILITY_HPP

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace utils
{

void								 print_container_map(std::map<std::string, std::string> myMap);
std::map<const char *, const char *> string_to_char(std::map<std::string, std::string> myMap);
char							   **cMap_to_cChar(std::map<std::string, std::string> myMap);
std::string							 convert_digit_into_string(int digit);
std::string							 toUpper(std::string s);

}
#endif // WEBSERV_UTILITY_HPP
