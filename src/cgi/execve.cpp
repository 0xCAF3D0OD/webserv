#include "../../inc/cgi.hpp"
#include "../../inc/std98.hpp"
#include "../../inc/utility.hpp"
#include "cgi.hpp"
#include "stdio.h"

#define BUFFER_SIZE 4092

std::string CGI::_path_php_binary = "";

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
	(void)bin;
	char *b;
	char *f;
	char *q;

	b = new char[_path_php_binary.size() + 1];
	f = new char[file.size() + 1];
	std::memcpy(b, _path_php_binary.c_str(), _path_php_binary.size() + 1);
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
	if (stat(_path_php_binary.c_str(), &sa) == 0)
		_script_name = _path_php_binary;

	std::map<std::string, std::string>::const_iterator it = map.find("Path");
	/*----- SCRIPT_FILENAME -----*/
	_script_path = "";
	if (it != map.end() && !it->second.empty())
	{
		struct stat sb = {};
		if (stat(name_file.c_str(), &sb) == 0)
			_script_path = name_file;
	}

	/*----- CONTENT_TYPE -----*/
	set_private_attribute(map.find("Content-Type"), map.end(), _cont_type);
	/*----- CONTENT_LENGTH -----*/
	set_private_attribute(map.find("Content-Length"), map.end(), _cont_length);
	/*----- PATH_INFO -----*/
	set_private_attribute(map.find("Path-Info"), map.end(), _location);
	/*----- PATH_TRANSLATED -----*/
	set_private_attribute(map.find("Path-Translated"), map.end(), _location);
	/*----- REQUEST_URI -----*/
	set_private_attribute(map.find("Request-Uri"), map.end(), _location);
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

void
CGI::set_php_binary(std::string const &path_binary)
{
	_path_php_binary = path_binary;
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
	_env["PATH_INFO"] = _location;
	// Chemin complet correspondant tel que supposé par le serveur, si PATH_INFO est présent.
	_env["PATH_TRANSLATED"] = _location;
	// L'URI qui a été donné pour accéder à cette page ; par exemple, "/index.html".
	// Uniform Resource Identifier (URI) utilisé dans HTTP pour identifier les ressources.
	_env["REQUEST_URI"] = _location;

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
}

std::string
CGI::parent_process(pid_t &pid)
{
	int ret;
	close(_pipefd[1]);
	if (waitpid(pid, &ret, 0) == -1)
	{
		perror("waitpid");
		throw(std::exception());
	}
	ssize_t bytes_read;
	do
	{
		// Fill _read_buffer with 0
		std::memset(_read_buffer, 0, BUFFER_SIZE);
		// Initialize bytes_read with the return value from read, for error checking.
		if ((bytes_read = read(_pipefd[0], _read_buffer, BUFFER_SIZE)) == -1)
		{
			perror("read");
			close(_pipefd[0]);
			exit(1);
		}
		// End of file
		else if (!bytes_read)
			break;
		// std::string output_cgi concatenation with append
		else
			_output_cgi.append(_read_buffer, bytes_read);
	} while (bytes_read == BUFFER_SIZE);
	// Close the process.
	close(_pipefd[0]);
	return (_output_cgi);
}

void
CGI::child_process(char **env)
{
	close(_pipefd[0]);
	// Replace the old FD
	if (dup2(_pipefd[1], STDOUT_FILENO) == -1)
		perror("dup2");
	close(_pipefd[1]);

	// Execute new process
	if (execve(_args[0], &_args[0], env) == -1)
		perror("execve");
	exit(1);
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
CGI::execution_cgi(const std::map<std::string, std::string> &map, const std::string &args)
{
	char **env;
	// Verify if pipe failed.
	if (pipe(_pipefd) == -1)
	{
		std::cerr << "error pipe" << std::endl;
		exit(1);
	}
	set_env(map, args);
	env = utils::cMap_to_cChar(_env);
	pid_t pid = fork();
	// Verify if fork failed
	if (pid == -1)
	{
		std::cerr << "error fork" << std::endl;
		exit(1);
	}
	else if (pid == 0)
		child_process(env);
	else
		parent_process(pid);
	while (!_args.empty())
	{
		delete[] _args.back();
		_args.pop_back();
	}
	free_env(env);
	return (_output_cgi);
}

CGI::~CGI(void){};
