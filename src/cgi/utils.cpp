#include "../../inc/cgi.hpp"
#include <iostream>
#include <map>
#include <string>

namespace utils
{
void
print_container_map(std::map<std::string, std::string> myMap)
{
	for (std::map<std::string, std::string>::iterator it = myMap.begin(); it != myMap.end(); ++it)
	{
		std::cout << "first: " << it->first << " second: " << it->second << std::endl;
	}
}

std::map<const char *, const char *>
string_to_char(std::map<std::string, std::string> myMap)
{
	std::map<const char *, const char *> myCMap;

	for (std::map<std::string, std::string>::iterator it = myMap.begin(); it != myMap.end(); ++it)
	{
		const char *key = it->first.c_str();
		const char *value = it->second.c_str();
		myCMap[key] = value;
	}
	// Now myCMap is a map of char* keys and values
	for (std::map<const char *, const char *>::iterator it = myCMap.begin(); it != myCMap.end(); ++it)
		std::cout << it->first << " = " << it->second << ", type first: " << typeid(it->first).name()
				  << ", type second: " << typeid(it->second).name() << std::endl;

	return (myCMap);
}

char **
cMap_to_cChar(std::map<std::string, std::string> myMap)
{
	std::map<std::string, std::string>::iterator it;

	// Allocation de l'espace mémoire
	char **array = new char *[myMap.size() + 1];
	size_t i = 0;
	// Concaténation des chaînes
	for (std::map<std::string, std::string>::iterator it = myMap.begin(); it != myMap.end(); ++it)
	{
		array[i] = new char[it->first.size() + it->second.size() + 4];
		std::string tmps = it->first + "=" + it->second;
		strcpy(array[i++], tmps.c_str());
	}
	array[i] = NULL;
	return (array);
}

std::string
toUpper(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	return (s);
}
}
