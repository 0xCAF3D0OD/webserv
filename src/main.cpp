#include "arguments.hpp"
#include "webserver.hpp"

#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>

#include <vector>

#define BS 10
#define BS_NULL 9

static void
binary_file(void)
{
	ssize_t			  bytes_read;
	char			  buffer[BS];
	int				  fd_src;
	std::string		  all_file_str;
	std::string		  all_file_str_append;
	std::vector<char> all_file_vec;

	fd_src = open("test/website/favicon.ico", O_RDONLY);
	if (fd_src == -1)
		return;

	std::ofstream file_dst("destination.ico", std::ios::trunc | std::ios::binary);
	std::ofstream file_dst_error("destination_failed.ico", std::ios::trunc | std::ios::binary);
	std::ofstream file_dst_string("destination_string.ico", std::ios::trunc | std::ios::binary);
	std::ofstream file_dst_vector("destination_vector.ico", std::ios::trunc | std::ios::binary);
	std::ofstream file_dst_string_append("destination_string_append.ico", std::ios::trunc | std::ios::binary);

	do
	{
		/* reset and read the buffer */
		std::memset(buffer, 0, BS);
		bytes_read = read(fd_src, buffer, BS_NULL);

		/* KO: append to a string */
		all_file_str += buffer;

		/* 4) Appends characters in the range [s, s + count). This range can contain null characters. */
		all_file_str_append.append(buffer, BS_NULL);

		/* OK: append to a vector*/
		all_file_vec.insert(all_file_vec.end(), buffer, &(buffer[bytes_read]));

		/* OK: write directly into the file */
		file_dst.write(buffer, bytes_read);

		/* KO: put unformated contents */
		file_dst_error << buffer;
	} while (bytes_read == BS_NULL);

	file_dst_string_append.write(all_file_str_append.c_str(), all_file_str_append.size());

	/* KO: write the string content into the file */
	file_dst_string.write(all_file_str.c_str(), all_file_str.size());

	/* OK: write the vector content into the file */
	file_dst_vector.write(all_file_vec.data(), all_file_vec.size());

	file_dst.close();
	file_dst_error.close();
	file_dst_string.close();
	file_dst_string_append.close();
	file_dst_vector.close();
	close(fd_src);
}

int
main(int argc, char **argv)
{
	binary_file();
	return 0;
	if (arguments::check_argc_number(argc))
		return 1;

	if (arguments::check_argv_access(argv[1]))
		return 1;

	return webserver(argv[1]);
}
