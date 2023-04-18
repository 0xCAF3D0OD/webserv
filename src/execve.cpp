#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

#define PIPE_RD 0
#define PIPE_WR 1
#define BUFFER_SIZE 3

// This value is equal to BUFFER_SIZE - 1
#define READ_SIZE 2

std::string
execve_cgi(char **arguments)
{
	int			pipefd[2];
	char		read_buffer[BUFFER_SIZE];
	pid_t		pid;
	std::string output_cgi;

	if (pipe(pipefd) == -1)
	{
		std::cerr << "error: pipe()" << std::endl;
		return ""; // TODO: throw an exception
	}

	pid = fork();
	if (pid == -1)
	{
		std::cerr << "error: fork()" << std::endl;
		close(pipefd[PIPE_RD]);
		close(pipefd[PIPE_WR]);
		return ""; // TODO: throw an exception
	}

	if (pid == 0) // child
	{
		close(pipefd[PIPE_RD]);
		dup2(pipefd[PIPE_WR], 1);
		close(pipefd[PIPE_WR]);
		execve(arguments[0], arguments, 0);
		std::cerr << "error: execve()" << std::endl;
	}
	else // parent
	{
		size_t count_read;
		close(pipefd[PIPE_WR]);
		waitpid(pid, 0, 0);
		do
		{
			std::memset(read_buffer, 0, BUFFER_SIZE);
			count_read = read(pipefd[PIPE_RD], read_buffer, READ_SIZE);
			output_cgi += read_buffer;

		} while (count_read == READ_SIZE);
		output_cgi += read_buffer;
		close(pipefd[PIPE_RD]);
		std::cout << "read: " << output_cgi << std::endl;
	}
	return output_cgi;
}
