#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Json.hpp"
#include "Socket.hpp"
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <sys/stat.h>
#include <vector>

class Cluster
{
  public:
	Cluster(void);
	~Cluster(void);
	void load_cluster(json::t_object *config);
	void setup();
	void run();
	void create_upload_folder(json::Value &config);

  private:
	typedef std::vector<json::Value> t_array;
	t_array							 _all_server_config;
	std::map<int, Socket *>			 _sockets;
	std::map<int, Socket *>			 _sockets_accepted;
	fd_set							 _master_fd_set;
	int								 _max_fd;
};

#endif
