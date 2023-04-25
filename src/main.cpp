#include "arguments.hpp"
#include "webserver.hpp"

#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>

#define BS 10
#define BS_NULL 9

static void
binary_file(void)
{
	ssize_t		bytes_read;
	char		buffer[BS];
	int			fd_src;
	std::string all_file;
	const char *file_path_src = "test/website/favicon.ico";
	const char *file_path_dst = "destination.ico";
	const char *file_path_dst_failed = "destination_failed.ico";
	const char *file_path_dst_string = "destination_string.ico";

	fd_src = open(file_path_src, O_RDONLY);
	if (fd_src == -1)
		return;

	std::ofstream file_dst(file_path_dst, std::ios::trunc | std::ios::binary);
	std::ofstream file_dst_error(file_path_dst_failed, std::ios::trunc | std::ios::binary);
	std::ofstream file_dst_string(file_path_dst_string, std::ios::trunc | std::ios::binary);

	do
	{
		std::memset(buffer, 0, BS);
		bytes_read = read(fd_src, buffer, BS_NULL);
		all_file += buffer; /* that not work */
		file_dst.write(buffer, bytes_read);
		file_dst_error << buffer; /* bad pratice because the output is unformatted */
	} while (bytes_read == BS_NULL);

	file_dst_string.write(all_file.c_str(), all_file.size()); /* not work */

	file_dst.close();
	file_dst_error.close();
	file_dst_string.close();
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
