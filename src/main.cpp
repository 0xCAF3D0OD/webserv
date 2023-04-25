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
	const char *file_path_src = "test/website/favicon.ico";
	const char *file_path_dst = "destination.ico";

	fd_src = open(file_path_src, O_RDONLY);
	if (fd_src == -1)
		return;

	std::ofstream file_dst(file_path_dst, std::ios::trunc | std::ios::binary);

	do
	{
		std::memset(buffer, 0, BS);
		bytes_read = read(fd_src, buffer, BS_NULL);
		file_dst.write(buffer, bytes_read);
	} while (bytes_read == BS_NULL);

	file_dst.close();
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
