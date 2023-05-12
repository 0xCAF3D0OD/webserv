#!/bin/sh

SETTING_HPP="test/inc/setting.hpp"

set_setting () {
	cat <<- eof > ${SETTING_HPP}
	#define PATH_CGI_PHP "$(which php-cgi || find ~/goinfre/ -name "php-cgi")"
	#define WHOAMI "$(whoami)"
	eof
}

main () {
	echo generate ${SETTING_HPP}
	set_setting
}

main
