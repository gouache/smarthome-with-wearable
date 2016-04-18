#!/usr/bin/env python

import os
import web
from socket import *
from select import select
import sys

HOST = ARDUINO_IP
PORT = ARDUINO_PORT
BUFSIZE = 512
ADDR = (HOST, PORT)

urls = (
    '/hello', 'hello',
    '/car', 'car'
)

app = web.application(urls, globals())
clientSocket = socket(AF_INET, SOCK_STREAM)

try:
    clientSocket.connect(ADDR)
except Exception as e:
    print('failed to connect arduino ' % ADDR)
    sys.exit()
print('succeed to connect arduino')

class hello:
    def GET(self):
        os.system('echo "hello"')
        output = "hello";
        return output

class open_door:
    def GET(self):
        clientSocket.send('#')
        output = "car";
        return output

if __name__ == "__main__":
    app.run()

