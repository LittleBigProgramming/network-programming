#!/usr/bin/python3

import sys
from socket import *

if len(sys.argv) < 3:
    sys.stderr.write("usage: %s host service args ...\n" % sys.argv[0])
    raise SystemExit(1)


s = socket(AF_INET, SOCK_STREAM)
port = getservbyname(sys.argv[2])
s.connect((sys.argv[1], port))

for message in sys.argv[3:] : 
    s.send(message.encode('ascii'))
    response = s.recv(len(message))
    print(response.decode(), end=" ")

s.close()
print()