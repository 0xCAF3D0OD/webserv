#!/bin/python3

import requests
import sys
import threading

exit_code = 0
TREADS_NUMBER = 15

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

def basic_status_code_test ():
    request_get("http://webserv.com:8082/index.html", 200)
    request_get("http://webserv.com:8082/", 200)
    request_get("http://webserv.com:8082/bad_page", 404)
    request_get("http://webserv.com:8082/uploads", 401)

    request_get("http://webserv.com:8081/index.html", 200)
    request_get("http://webserv.com:8081/", 200)
    request_get("http://webserv.com:8081/bad_page", 404)
    request_get("http://webserv.com:8081/uploads", 401)
    request_get("http://webserv.com:8081/favicon.ico", 200)

    request_get("http://webserv.com:8080/index.html", 405)
    request_get("http://webserv.com:8080/", 200)
    request_get("http://webserv.com:8080/bad_page", 404)
    request_get("http://webserv.com:8080/uploads", 401)
    request_get("http://webserv.com:8080/favicon.ico", 404)

def try_one():
    global exit_code
    print ("one")

    url = "http://localhost:8081/uploads"
    filename = "not-real-png.png"
    myfiles = {"file": (filename, open(filename, "rb"))}

    try:
        response = requests.post(url, files=myfiles)
        print(response.text)
    except Exception as err:
        exit_code = 1
        print("KO post")

def try_two():
    global exit_code
    print ("two")

    url = "http://localhost:8081/uploads"
    filename = "not-real-png.png"
    myfiles = {"file": (filename, open(filename, "rb"), 'image/png')}

    try:
        response = requests.post(url, files=myfiles)
        print(response.text)
    except Exception as err:
        exit_code = 1
        print("KO post")

def my_thread (name):
    print(f"tread number {name}")
    basic_status_code_test()

def multi_request ():
    all_treads = []

    for i in range(TREADS_NUMBER):
        all_treads.append(threading.Thread(target=my_thread, args=[i]))
    for i in range(TREADS_NUMBER):
        all_treads[i].run()

def main ():
    multi_request()

def main():
    print ("main")
    try_one()
    try_two()


if __name__ == "__main__":
    main()
    sys.exit(exit_code)
