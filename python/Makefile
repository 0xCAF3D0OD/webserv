build:
	docker build . -t my-python

test:
	docker run \
	--name python-http \
	--network bridge \
	--add-host webserv.com:host-gateway \
	--rm  my-python \

exec:
	docker run --tty \
	--name python-http \
	--network bridge \
	--add-host webserv.com:host-gateway \
	--rm  my-python \
	python
