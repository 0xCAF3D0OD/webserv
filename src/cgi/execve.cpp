#include "../../inc/cgi.hpp"
#include "../../inc/std98.hpp"
#include "../../inc/utility.hpp"
#include "cgi.hpp"
#include "stdio.h"

#define BUFFER_SIZE 4092

std::string
get_key(const std::map<std::string, std::string> &map, const std::string &key)
{
	std::map<std::string, std::string>::const_iterator it = map.find(key);
	if (it != map.end() && !it->second.empty())
		return it->second;
	return "";
}

CGI::CGI(void) {}

CGI::CGI(const std::string &bin, const std::string &file, const std::string &query)
{
	char *b;
	char *f;
	char *q;

	b = new char[bin.size() + 1];
	f = new char[file.size() + 1];
	std::memcpy(b, bin.c_str(), bin.size() + 1);
	std::memcpy(f, file.c_str(), file.size() + 1);
	_args.push_back(b);
	_args.push_back(f);
	if (!query.empty())
	{
		q = new char[query.size() + 1];
		std::memcpy(q, query.c_str(), query.size() + 1);
		_args.push_back(q);
	}
	_args.push_back(NULL);
}

static void
set_private_attribute(const std::map<std::string, std::string>::const_iterator &it,
					  const std::map<std::string, std::string>::const_iterator end, std::string &attribute)
{
	if (it != end && !it->second.empty())
		attribute = it->second;
	else
		attribute = "";
}
void
CGI::check_map(const std::map<std::string, std::string> &map, const std::string &name_file)
{
	/*----- SCRIPT_NAME -----*/
	struct stat sa = {};
	_script_name = "";
	if (stat("/Users/kdi-noce/goinfre/php/php-8.2.5/sapi/cgi/php-cgi", &sa) == 0)
		_script_name = "/Users/kdi-noce/goinfre/php/php-8.2.5/sapi/cgi/php-cgi";

	std::map<std::string, std::string>::const_iterator it = map.find("Path");
	/*----- SCRIPT_FILENAME -----*/
	_script_path = "";
	if (it != map.end() && !it->second.empty())
	{
		struct stat sb = {};
		if (stat(name_file.c_str(), &sb) == 0)
			_script_path = name_file;
	}

	it = map.find("Content-Length");
	if (it != map.end() && !it->second.empty())
		_cont_length = it->second;
	else
		_cont_length = "0";

	/*----- CONTENT_TYPE -----*/
	set_private_attribute(map.find("Content-Type"), map.end(), _cont_type);
	/*----- REQUEST_METHOD -----*/
	set_private_attribute(map.find("Method"), map.end(), _method);
	/*----- QUERY_STRING -----*/
	set_private_attribute(map.find("Query"), map.end(), _query);
	/*----- SERVER_NAME -----*/
	set_private_attribute(map.find("Server-Name"), map.end(), _env_Host);
	/*----- SERVER_PROTOCOLE -----*/
	set_private_attribute(map.find("Server-Protocole"), map.end(), _http_version);
	/*----- SERVER_PORT -----*/
	set_private_attribute(map.find("Port"), map.end(), _port);
}

/*
 * Environment variables in PHP-CGI serve as a way to provide configuration values, system
 * information, and sensitive data to PHP scripts during their execution, allowing for better separation of
 * concerns, improved security, and easier management of dependencies and configurations.
 */

void
CGI::set_env(const std::map<std::string, std::string> &map, const std::string &script_name)
{

	check_map(map, script_name);

	// Définit la route par laquelle tous les utilisateurs seront authentifiés
	// (dans les applications prises en charge).
	_env["AUTH_TYPE"] = "";
	// Type de média Internet des données d'entrée si la méthode PUT ou POST est utilisée, tel que fourni par
	// l'en-tête HTTP.
	_env["CONTENT_TYPE"] = _cont_type;
	// Taille des données d'entrée (décimale, en octets) si elles sont fournies via la page HTTP.
	std::string len = _cont_length;
	_env["CONTENT_LENGTH"] = len;

	// Donne la version du CGI utilisé.
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	// Contient toutes les informations sur le chemin d'accès fournies par le client, après le nom de fichier
	// du script mais avant la chaîne de requête, si elles sont disponibles.
	// Par exemple, si le script actuel a été accédé via l'URI
	// http://www.example.com/php/path_info.php/some/stuff?foo=bar,
	// $_SERVER['PATH_INFO'] contiendra /some/stuff.
	_env["PATH_INFO"] = _script_name;
	// Chemin complet correspondant tel que supposé par le serveur, si PATH_INFO est présent.
	_env["PATH_TRANSLATED"] = _script_name;
	// L'URI qui a été donné pour accéder à cette page ; par exemple, "/index.html".
	// Uniform Resource Identifier (URI) utilisé dans HTTP pour identifier les ressources.
	_env["REQUEST_URI"] = _script_name;

	// Donne l'IP ou le DNS du serveur.
	_env["SERVER_NAME"] = _env_Host;
	// Donne le nom et la version du protocole utilisé par le serveur et le client.
	// Cette variable contient généralement une chaîne de texte telle que "HTTP/1.1" ou "HTTP/2.0",
	// qui indique la version du protocole utilisée.
	_env["SERVER_PROTOCOL"] = _http_version;
	// TCP port (decimal).
	_env["SERVER_PORT"] = _port;

	// Donne la valeur de l'attribut METHOD du marqueur FORM utilisé lors de la requête CGI.
	// En gros le type de requête http.
	_env["REQUEST_METHOD"] = _method;
	// Chemin relatif vers le programme, comme /cgi-bin/script.cgi.
	_env["SCRIPT_NAME"] = _script_name;
	// Chemin absolu vers le programme, comme /cgi-bin/script.cgi.
	_env["SCRIPT_FILENAME"] = _script_path;
	// Donne les informations passées par un marqueur <FORM METHOD=GET>
	_env["QUERY_STRING"] = _query;
	// Très utile si vous faites pointer toutes vos pages d'erreur vers le même fichier.
	_env["REDIRECT_STATUS"] = "200";

	std::map<std::string, std::string> map1 = map;
	for (std::map<std::string, std::string>::iterator i = map1.begin(); i != map1.end(); i++)
	{
		if (!i->second.empty())
		{
			std::string header = "HTTP_" + utils::toUpper(i->first);
			std::replace(header.begin(), header.end(), '-', '_');
			_env[header] = i->second;
			std::cout << _env[header] << std::endl;
		}
	}
}

static void
free_env(char **env)
{
	for (unsigned int index = 0; env[index]; ++index)
	{
		delete[] env[index];
	}
	delete[] env;
}

std::string
CGI::execution_cgi(const std::map<std::string, std::string> &map, const std::string &args,
				   const std::string &body_post_cgi)
{
	char	  **env;
	int			p_out[2];
	int			p_in[2];
	int			ret;
	char		buffer[4096];
	std::string result;

	pipe(p_out);
	pipe(p_in);

	fcntl(p_out[0], F_SETFL, O_NONBLOCK);
	fcntl(p_out[1], F_SETFL, O_NONBLOCK);
	fcntl(p_in[0], F_SETFL, O_NONBLOCK);
	fcntl(p_in[1], F_SETFL, O_NONBLOCK);

	set_env(map, args);
	env = utils::cMap_to_cChar(_env);
	//	std::vector<char *> path = preparePath(request.getQuery());
	//	std::map<std::string, std::string> env = prepareEnv(request);

	int pid = fork();

	if (pid == 0)
	{
		close(p_out[0]);
		dup2(p_out[1], STDOUT_FILENO);
		close(p_out[1]);

		close(p_in[1]);
		dup2(p_in[0], STDIN_FILENO);
		close(p_in[0]);

		execve(_args[0], &_args[0], env);
		exit(0);
	}
	else
	{
		close(p_in[0]);
		close(p_out[1]);

		std::cout << "getbody '[" << body_post_cgi.c_str() << "]'" << std::endl;
		write(p_in[1], body_post_cgi.c_str(), body_post_cgi.size());
		// std::cout << request.getBody() << "\n";
		close(p_in[1]);

		if (waitpid(pid, &ret, 0) == -1)
			throw(std::exception());
		if (ret != 0)
		{
			result.append("Status: 502 Error in CGI application\r\n");
		}

		do
		{
			bzero(&buffer, sizeof(buffer));
			ret = read(p_out[0], buffer, sizeof(buffer));
			if (ret > 0)
				result.append(std::string(buffer, ret));
		} while (ret == sizeof(buffer));
		close(p_out[0]);
	}
	while (!_args.empty())
	{
		delete[] _args.back();
		_args.pop_back();
	}

	free_env(env);
	return (result);
}
//
// std::string
// CGI::parent_process(pid_t &pid, const std::string& body_post_cgi)
//{
//	int ret;
//	close(_p_in[0]);
//	close(_p_out[1]);
//
//	write(_p_in[1], body_post_cgi.c_str(), body_post_cgi.size());
//
//	close(_p_in[1]);
//	if (waitpid(pid, &ret, 0) == -1)
//	{
//		perror("waitpid");
//		throw(std::exception());
//	}
//
//	ssize_t bytes_read;
//	do
//	{
//		// Fill _read_buffer with 0
//		std::memset(_read_buffer, 0, BUFFER_SIZE);
//		// Initialize bytes_read with the return value from read, for error checking.
//		if ((bytes_read = read(_p_out[0], _read_buffer, BUFFER_SIZE)) == -1)
//		{
//			perror("read");
//			close(_p_out[0]);
//			exit(1);
//		}
//		// End of file
//		else if (!bytes_read)
//			break;
//		// std::string output_cgi concatenation with append
//		else
//			_output_cgi.append(_read_buffer, bytes_read);
//	} while (bytes_read == BUFFER_SIZE);
//	// Close the process.
//	close(_p_out[0]);
//	return (_output_cgi);
//}
//
// void
// CGI::child_process(char **env)
//{
//	// Replace the old FD
//	close(_p_out[0]);
//	if (dup2(_p_out[1], STDOUT_FILENO) == -1)
//		perror("dup2");
//	close(_p_out[1]);
//	close(_p_in[1]);
//	if (dup2(_p_in[0], STDIN_FILENO) == -1)
//		perror("dup2");
//	close(_p_in[0]);
//	// Execute new process
//	if (execve(_args[0], &_args[0], env) == -1)
//		perror("execve");
//	exit(1);
//}
//
// static void
// free_env(char **env)
//{
//	for (unsigned int index = 0; env[index]; ++index)
//	{
//		delete[] env[index];
//	}
//	delete[] env;
//}
//
// std::string
// CGI::execution_cgi(const std::map<std::string, std::string> &map, const std::string &args, const
// std::string& body_post_cgi)
//{
//	char **env;
//	// Verify if pipe failed.
//	if (pipe(_p_in) == -1 || pipe(_p_out) == -1)
//	{
//		std::cerr << "error pipe" << std::endl;
//		exit(1);
//	}
//	set_env(map, args);
//	env = utils::cMap_to_cChar(_env);
//
//	fcntl(_p_out[0], F_SETFL, O_NONBLOCK);
//	fcntl(_p_out[1], F_SETFL, O_NONBLOCK);
//	fcntl(_p_in[0], F_SETFL, O_NONBLOCK);
//	fcntl(_p_in[1], F_SETFL, O_NONBLOCK);
//
//	pid_t pid = fork();
//	// Verify if fork failed
//	if (pid == -1)
//	{
//		std::cerr << "error fork" << std::endl;
//		exit(1);
//	}
//	else if (pid == 0)
//		child_process(env);
//	else
//		parent_process(pid, body_post_cgi);
//	while (!_args.empty())
//	{
//		delete[] _args.back();
//		_args.pop_back();
//	}
//	free_env(env);
//	return (_output_cgi);
//}

CGI::~CGI(void){};
