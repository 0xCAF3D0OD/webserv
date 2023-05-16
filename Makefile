NAME	=	webserv
LIB		=	lib$(NAME).a


CXX		=	g++
AR		=	ar rs 2> /dev/null
DB		=	lldb
MAKE	=	make --no-print-directory 1> /dev/null
ADOC	=	asciidoctor --require=asciidoctor-diagram


DOCU	=	docs/README.adoc
INDEX	=	docs/index.html
MANUAL	=	man/webserv.adoc


CFLAGS	=	-Wall
CFLAGS	+=	-Wextra
CFLAGS	+=	-Werror
CFLAGS	+=	-g
CFLAGS	+=	-Wfatal-errors
CFLAGS	+=	-std=c++98

OFLAGS	=	-fsanitize=address


#	Sources

SRCD	=	src

SRCS	=	$(SRCD)/main.cpp
SRCS	+=	$(SRCD)/webserver.cpp
SRCS	+=	$(SRCD)/arguments/check_argc_number.cpp
SRCS	+=	$(SRCD)/arguments/check_argv_access.cpp
SRCS	+=	$(SRCD)/Socket/Socket.cpp

# cgi

SRCS	+=	$(SRCD)/cgi/execve.cpp
SRCS	+=	$(SRCD)/cgi/utils.cpp

# http

SRCS	+=	$(SRCD)/http/Method.cpp
SRCS	+=	$(SRCD)/http/Redirection.cpp
SRCS	+=	$(SRCD)/http/Request.cpp
SRCS	+=	$(SRCD)/http/Response.cpp
SRCS	+=	$(SRCD)/http/StatusCodes.cpp

# json

SRCS	+=	$(SRCD)/json/File.cpp
SRCS	+=	$(SRCD)/json/read.cpp
SRCS	+=	$(SRCD)/json/load_config.cpp
SRCS	+=	$(SRCD)/json/check_config.cpp
SRCS	+=	$(SRCD)/json/process_line.cpp
SRCS	+=	$(SRCD)/json/utils.cpp
SRCS	+=	$(SRCD)/json/Value.cpp

# std98

SRCS	+=	$(SRCD)/std98/to_string.cpp

# cluster

SRCS	+=	$(SRCD)/cluster/cluster.cpp

# utils

SRCS	+=  $(SRCD)/utils/my_replace.cpp

#	Headers

INCD	=	inc

#	Objets

OBJD	=	obj
OBJS	=	$(patsubst $(SRCD)/%.cpp, $(OBJD)/%.o, $(SRCS))

RM		=	rm -rf


all : $(NAME)

$(NAME):	$(OBJS)
	@printf "$(YELLOW)Creating executable..$(DEFAULT)\n"
	@$(CXX) $(OBJS) $(OFLAGS) -o $(NAME)
	@printf "$(GREEN)---> $(NAME) is ready$(DEFAULT)\n"

$(OBJD)/%.o: $(SRCD)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) -c $(<) $(CFLAGS) -I$(INCD) -o $(@)

$(LIB): $(OBJS)
	@printf "$(YELLOW)Creating $(LIB)..$(DEFAULT)\n"
	@$(AR) $(LIB) $(OBJS)
	@printf "$(GREEN)---> $(LIB) is ready$(DEFAULT)\n"
	@printf "$(YELLOW)make re test..$(DEFAULT)\n"
	@$(MAKE) --directory=test re
	@printf "$(GREEN)---> test$(DEFAULT)\n"

test: $(LIB) setting test-cpp test-python

test-cpp:
	@$(MAKE) --directory=test
	@printf "$(YELLOW)Launching test..$(DEFAULT)\n"
	@(test/test && printf "$(GREEN)test: SUCCESS$(DEFAULT)\n") || printf "$(RED)test: ERROR$(DEFAULT)\n"

test-python:
	@$(MAKE) --directory=python 2>/dev/null
	@(docker run \
	--name python-http \
	--network bridge \
	--volume $(shell pwd):/workdir \
	--add-host webserv.com:host-gateway \
	--rm  my-python && \
	printf "$(GREEN)test: SUCCESS$(DEFAULT)\n") || printf "$(RED)test: ERROR$(DEFAULT)\n"

setting:
	@if ! grep $(shell whoami) test/inc/setting.hpp 1>/dev/null; then sh setting.sh ;fi


db: $(NAME)
	$(DB) $(NAME)

format:
	@printf "$(YELLOW)Formating the sources..$(DEFAULT)\n"
	@clang-format -i $(shell find . -name "*.hpp" -or -name "*.cpp" -or -name "*.js")

formatdocker:
	@printf "$(YELLOW)launch tgrivel/clang-format docker image..$(DEFAULT)\n"
	@docker run --rm -v $(shell pwd):/documents tgrivel/clang-format -i $(shell find . -name "*.hpp" -or -name "*.cpp" -or -name "*.js")

doc:
	@printf "$(YELLOW)Generating documentations..$(DEFAULT)\n"
	@$(ADOC) $(DOCU) -o $(INDEX)

docdocker:
	@printf "$(YELLOW)launch the asciidoctor/docker-asciidoctor docker image..$(DEFAULT)\n"
	@docker run --rm -v $(shell pwd):/documents/ asciidoctor/docker-asciidoctor make doc

man:
	@printf "$(YELLOW)Generating manual..$(DEFAULT)\n"
	@$(ADOC) -b manpage $(MANUAL)

mandocker:
	@printf "$(YELLOW)launch the asciidoctor/docker-asciidoctor docker image..$(DEFAULT)\n"
	@docker run --rm -v $(shell pwd):/documents/ asciidoctor/docker-asciidoctor make man

clean:
	@$(RM) $(OBJD)
	@printf "$(RED)Removed $(CYAN)$(OBJD)$(DEFAULT)\n"

fclean: clean
	@$(RM) $(NAME)
	@$(RM) $(LIB)
	@printf "$(RED)Removed $(CYAN)$(NAME)$(DEFAULT)\n"

fullclean: fclean
	@$(MAKE) --directory=test fclean
	@$(MAKE) --directory=test/framework fclean

re:	fclean all


.PHONY: all clean fclean libclean fullclean test doc man


#COLORS
RED = \033[1;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
CYAN = \033[1;36m
DEFAULT = \033[0m
