#!/bin/python3

from requests_toolbelt import MultipartEncoder
import requests
import sys
import threading

exit_code = 0
TREADS_NUMBER = 15

# From test/website/submitPage.html:
# try as fild name different name
#
# <input
# type="file"
# name="pics"
# id="pics-input"
# accept="image/*, .pdf" />

#field_name = "pics"
field_name = "pics-input"
the_png_file = "pp.png"
#field_name = "pics"

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
    filename = the_png_file
    myfiles = {field_name: (filename, open(filename, "rb"))}

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
    filename = the_png_file
    myfiles = {field_name: (filename, open(filename, "rb"), 'image/png')}

    try:
        response = requests.post(url, files=myfiles)
        print(response.text)
    except Exception as err:
        exit_code = 1
        print("KO post")

def try_tree():
    global exit_code
    print ("three")

    url = "http://localhost:8081/uploads"
    filename = the_png_file
    myfiles =  MultipartEncoder(
    fields={field_name: ('filename', open(the_png_file, 'rb'), 'image/png')}
    )

    try:
        response = requests.post(url, data=myfiles, headers={'Content-Type': m.content_type})
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
    print ("main")
    print("field name: " + field_name)
    print("file name : " + the_png_file)
    print ("--------------------------------------------------------------------------------")
    try_one()
    print ("--------------------------------------------------------------------------------")
    try_two()
    print ("--------------------------------------------------------------------------------")
    try_tree()


if __name__ == "__main__":
    main()
    sys.exit(exit_code)
