#include "arguments.hpp"
#include "webserver.hpp"

#include <iostream>
#include <sys/socket.h>

#include "Request.hpp"
#include "Response.hpp"
#include <climits>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define LISTEN_BACKLOG 10

static void
main_loop(void)
{
	int				   val(1);
	unsigned short	   port(8080);
	struct sockaddr_in _address;

	_address.sin_family = AF_INET;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);

	int socket_descriptor(socket(AF_INET, SOCK_STREAM, 0));

	if (socket_descriptor == -1)
	{
		std::cerr << "error: socket" << std::endl;
		return;
	}

	if (setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)))
	{
		std::cerr << "error: setsockopt" << std::endl;
		return;
	}

	if (bind(socket_descriptor, reinterpret_cast<struct sockaddr *>(&_address), sizeof(_address)))
	{
		std::cerr << "error: bind" << std::endl;
		return;
	}

	if (listen(socket_descriptor, LISTEN_BACKLOG))
	{
		std::cerr << "error: listen" << std::endl;
		return;
	}

	std::cout << "++++ accept() ++++" << std::endl;
	int descriptor_accepted(accept(socket_descriptor, NULL, NULL) == -1);
	if (descriptor_accepted == -1)
	{
		std::cerr << "error: accept" << std::endl;
		return;
	}

	ssize_t bytes_read;
	do
	{
		char buffer[1000];
		std::memset(buffer, 0, 1000);
		bytes_read = recv(socket_descriptor, buffer, 999, 0);
		std::cout << buffer << std::endl;

	} while (bytes_read == 999);

	close(socket_descriptor);
}

int
main(int argc, char **argv)
{
	main_loop();
	return 0;

	if (arguments::check_argc_number(argc))
		return 1;

	if (arguments::check_argv_access(argv[1]))
		return 1;

	return webserver(argv[1]);
}
