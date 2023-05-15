#!/bin/python3

import requests
import sys

exit_code = 0

class bcolors:
    HEADER    = '\033[95m'
    OKBLUE    = '\033[94m'
    OKCYAN    = '\033[96m'
    OKGREEN   = '\033[92m'
    WARNING   = '\033[93m'
    FAIL      = '\033[91m'
    ENDC      = '\033[0m'
    BOLD      = '\033[1m'
    UNDERLINE = '\033[4m'

def print_error(message):
    print(bcolors.FAIL + message + bcolors.ENDC)

def print_success(message):
    print(bcolors.OKGREEN + message + bcolors.ENDC)

def request_get (url, expected_status_code):
    global exit_code
    print("Request get on: " + url)

    try:
        request_index = requests.get(url)
    except Exception as e:
        print ("error: script.py not connectable")
        sys.exit(1)

    if request_index.status_code != expected_status_code:
        print_error(f"[KO] : http status code : {request_index.status_code} / {expected_status_code}")
        exit_code = 1
    else:
        print_success(f"[OK] : http status code : {request_index.status_code} / {expected_status_code}")

    print()

def redirection (url, url_redirection):
    global exit_code
    print("test redirecion of: " + url)

    try:
        request_index = requests.get(url)
    except Exception as e:
        print ("error: script.py not connectable")
        sys.exit(1)

    print("length of history: " + str(len(request_index.history)))
    for count, red in enumerate(request_index.history):
        print(f"history[{count}] -- status code: {red.status_code} url: {red.url}")
        print(red.request.headers)
    if request_index.history[0].status_code != 301:
        print_error(f"[KO] : http status code : {request_index.history[0].status_code} / 301")
        exit_code = 1
    else:
        print_success(f"[OK] : http status code : {request_index.history[0].status_code} / 301")

    if len(request_index.history) < 2:
        exit_code = 1
        print_error("Error: The length of history is less than 2")
        return
    if request_index.history[1].url != url_redirection:
        print_error(f"[KO] : http redirection url : {request_index.history[1].url} != {url_redirection}")
        exit_code = 1
    else:
        print_success(f"[OK] : http redirection url : {request_index.history[1].url} == {url_redirection}")

    print()

def main ():
    request_get("http://webserv.com:8082/index.html", 200)
    request_get("http://webserv.com:8082/", 200)
    request_get("http://webserv.com:8082/bad_page", 404)
    request_get("http://webserv.com:8082/uploads", 401)

    request_get("http://webserv.com:8081/index.html", 200)
    request_get("http://webserv.com:8081/", 200)
    request_get("http://webserv.com:8081/bad_page", 404)
    request_get("http://webserv.com:8081/uploads", 401)
    request_get("http://webserv.com:8081/favicon.ico", 200)

    request_get("http://webserv.com:8080/index.html", 200)
    request_get("http://webserv.com:8080/", 200)
    request_get("http://webserv.com:8080/bad_page", 404)
    request_get("http://webserv.com:8080/uploads", 401)
    request_get("http://webserv.com:8080/favicon.ico", 404)

    redirection("http://webserv.com:8081/site_duck", "https://www.duckduckgo.com")
    redirection("http://webserv.com:8081/site_sheep", "https://www.google.com")

if __name__ == "__main__":
    main()
    sys.exit(exit_code)
