#!/usr/bin/env python3

"""
This script generates manages licenses.
It allows someone to add licenses, or list all the current licenses.
"""
import logging
import random
import socket
import select

LICENSE_FILE = "./.license_keys"
SERVER_PORT = 1123


def generate_license():
    KEY = '0123456789ABCDEF'

    key = ''
    for j in range(1, 5):
        for i in range(0, 8):
            key += str(KEY[random.randint(1, len(KEY) - 1)])
        if j != 4:
            key += '-'
    return (key)


def find_license(key):
    # Check if a license key is in the license file
    try:
        rc = open(LICENSE_FILE, 'r').read().find(key)
    except Exception as e:
        return False
    return rc != -1


def add_license(clientsocket):
    # Add a license key to the license file
    key = generate_license()

    # Loops until the license is new
    while find_license(key):
        key = generate_license()

    print(key)
    with open(LICENSE_FILE, 'a') as f:
        f.write(key + '\n')
    totalsent = 0
    msg = "License key file changed"
    while totalsent < len(msg):
        sent = clientsocket.send(msg[totalsent:].encode())
        if sent == 0:
            raise RuntimeError("Socket connection broken")
        totalsent = totalsent + sent


def list_license():
    try:
        with open(LICENSE_FILE, 'r') as f:
            print(f.read(), end='')
    except Exception as e:
        print (e)


# Script starts here
logging.basicConfig(format='%(asctime)s %(message)s',
                    datefmt='%m/%d/%Y %I:%M:%S %p', level=logging.INFO)

logging.info('Starting license manager.')
logging.info('Writing to file ' + LICENSE_FILE)

# Create socket
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
serversocket.bind(('localhost', SERVER_PORT))
serversocket.listen(1)
logging.info('Local server listening on port ' + str(SERVER_PORT))


# Waiting for ConnectManager connection
(clientsocket, address) = serversocket.accept()
logging.info('Client connected !')

# License manager loop
try:
    while True:
        # Process user input
        print('> ', end='')

        # Get input
        msg = input()
        logging.debug('User typed: ' + msg)

        try:
            ready_to_read, ready_to_write, in_error = select.select(
                [clientsocket], [clientsocket, ], [], 5)
        except select.error:
            # 0 = done receiving, 1 = done sending, 2 = both
            clientsocket.shutdown(2)
            clientsocket.close()
            # connection error event here
            print('Connection error')
            break

        # If there's data to read, disconnect
        if len(ready_to_read) > 0:
            break

        # Process input
        if msg == 'exit':
            break
        elif msg == 'help':
            print('Available commands: exit / help / list / add')
        elif msg == 'list':
            list_license()
        elif msg == 'add':
            add_license(clientsocket)
        else:
            print('Invalid command')
except Exception as e:
    print(e)

logging.info('Stopping license manager.')
