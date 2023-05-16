#include "cluster.hpp"

Cluster::Cluster(void) {}

Cluster::~Cluster(void) {}

void
Cluster::create_upload_folder(json::Value &config)
{
	std::string website_path(config.get("path").get<std::string>() + "/uploads");

	mkdir(website_path.c_str(), S_IRWXU);
}

void
Cluster::load_cluster(json::t_object *config)
{
	const json::Value empty_value;

	for (json::t_object::iterator this_server_config(config->begin()); this_server_config != config->end();
		 ++this_server_config)
	{
		if (this_server_config->second.get_type() == json::JSON_OBJECT)
		{
			_all_server_config.push_back(empty_value);
			_all_server_config.back().duplicate(this_server_config->second.get<json::t_object>());
			create_upload_folder(_all_server_config.back());
		}
	}
}

void
Cluster::setup()
{
	FD_ZERO(&_master_fd_set);
	_max_fd = 0;
	for (t_array::iterator it(_all_server_config.begin()); it != _all_server_config.end(); it++)
	{
		try
		{
			Socket *new_socket = new Socket(AF_INET, it->get("port").get<double>(), SOCK_STREAM, 0, *it);
			int		fd = new_socket->get_socket_id();
			FD_SET(fd, &_master_fd_set);
			_sockets.insert(std::make_pair(fd, new_socket));
			if (fd > _max_fd)
				_max_fd = fd;
			std::cout << it->get("hostname").get<std::string>() << ":" << it->get("port").get<double>()
					  << std::endl;
		}
		catch (std::exception &e)
		{
			std::cerr << it->get("hostname").get<std::string>() << " => Socket error, couldn't create it."
					  << std::endl;
		}
	}
}

void
Cluster::run()
{
	int end_server = 0;
	while (end_server == 0)
	{
		fd_set reading_set;
		int	   select_return = 0;

		while (select_return == 0)
		{
			FD_ZERO(&reading_set);
			std::memcpy(&reading_set, &_master_fd_set, sizeof(_master_fd_set));
			std::time_t result = std::time(nullptr);
			std::cout << std::asctime(std::localtime(&result)) << "++++++ Waiting for new connection ++++++"
					  << std::endl;
			select_return = select(_max_fd + 1, &reading_set, NULL, NULL, NULL);
		}
		if (select_return > 0)
		{
			int max_search = select_return;
			for (int i = 0; i <= _max_fd && max_search; i++)
			{
				if (FD_ISSET(i, &reading_set))
				{
					std::map<int, Socket *>::iterator it = _sockets.find(i);
					if (it != _sockets.end())
					{
						int new_sd = it->second->socket_accept();
						if (new_sd > 0)
							_sockets_accepted.insert(std::make_pair(new_sd, it->second));
						if (new_sd > _max_fd)
							_max_fd = new_sd;
						max_search--;
					}
				}
			}
			std::map<int, Socket *>::iterator it = _sockets_accepted.begin();
			while (it != _sockets_accepted.end())
			{
				int ret;

				ret = it->second->socket_recv();
				if (ret == -1)
				{
					it = _sockets_accepted.begin();
					continue;
				}
				if (ret == 0)
				{
					close(it->first);
				}
				_sockets_accepted.erase(it);
				it = _sockets_accepted.begin();
			}
		}
		else if (select_return < 0)
		{
			std::cerr << "Error with select()" << std::endl;
			_sockets_accepted.clear();
			for (std::map<int, Socket *>::iterator it(_sockets.begin()); it != _sockets.end(); it++)
			{
				delete it->second;
			}
			_sockets.clear();
			end_server = 1;
		}
	}
}
