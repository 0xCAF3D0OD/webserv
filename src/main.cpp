#include "arguments.hpp"
#include "webserver.hpp"

#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define BS 10
#define BS_NULL 9

static void
binary_file(void)
{
	ssize_t		bytes_read;
	char		buffer[BS];
	int			fd;
	const char *file_path = "src/main.cpp";

	fd = open(file_path, O_RDONLY);
	if (fd == -1)
		return;

	do
	{
		std::memset(buffer, 0, BS);
		bytes_read = read(fd, buffer, BS_NULL);
		printf("%s", buffer);
	} while (bytes_read == BS_NULL);

	close(fd);
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
