#!/bin/python3

from requests_toolbelt import MultipartEncoder
import requests
import sys

exit_code = 0

# From test/website/submitPage.html:
# try as fild name different name
#
# <input
# type="file"
# name="pics"
# id="pics-input"
# accept="image/*, .pdf" />

all_field_name = ["pics", "pics-input", "file"]
the_png_file = "pp.png"

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

def print_response(response):
    print (f"status code: {response.status_code}")
    print (response.headers)
    print (response.request.headers)

def try_one():
    global exit_code
    print ("one")

    url = "http://webserv.com:6060/uploads"
    filename = the_png_file
    myfiles = {field_name: (filename, open(filename, "rb"))}

    try:
        response = requests.post(url, files=myfiles)
        print_response(response)
    except Exception as err:
        exit_code = 1
        print_error("KO post")

def try_two():
    global exit_code
    print ("two")

    url = "http://webserv.com:6060/uploads"
    filename = the_png_file
    myfiles = {field_name: (filename, open(filename, "rb"), 'image/png')}

    try:
        response = requests.post(url, files=myfiles)
        print_response(response)
    except Exception as err:
        exit_code = 1
        print_error("KO post")

def try_tree():
    global exit_code
    print ("three")

    url = "http://webserv.com:6060/uploads"
    filename = the_png_file
    myfiles =  MultipartEncoder(
    fields={field_name: ('filename', open(the_png_file, 'rb'), 'image/png')}
    )

    try:
        response = requests.post(url, data=myfiles)
        print_response(response)
    except Exception as err:
        exit_code = 1
        print_error("KO post")

def try_four():
    global exit_code
    print ("four")

    url = "http://webserv.com:6060/uploads"
    filename = the_png_file
    myfiles = {field_name: (filename, open(filename, "rb"), 'multipart/form-data')}

    try:
        response = requests.post(url, files=myfiles)
        print_response(response)
    except Exception as err:
        exit_code = 1
        print_error("KO post")

def try_five():
    global exit_code
    print ("five")

    url = "http://webserv.com:6060/app.js"
    filename = the_png_file
    data = { "Button": "Submit" }
    myfiles = {field_name: (filename, open(filename, "rb"), 'multipart/form-data')}

    try:
        response = requests.post(url, files=myfiles, data=data)
        print_response(response)
    except Exception as err:
        exit_code = 1
        print_error("KO post")

def main():
    print ("main")
    global field_name
    for name_field in all_field_name:
        field_name = name_field
        print ("================================================================================")
        print("field name: " + field_name)
        print("file name : " + the_png_file)
        print ("--------------------------------------------------------------------------------")
        try_one()
        print ("--------------------------------------------------------------------------------")
        try_two()
        print ("--------------------------------------------------------------------------------")
        try_tree()
        print ("--------------------------------------------------------------------------------")
        try_four()
        print ("--------------------------------------------------------------------------------")
        try_five()


if __name__ == "__main__":
    main()
    sys.exit(exit_code)
