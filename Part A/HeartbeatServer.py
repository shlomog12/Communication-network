import random
import sys
from socket import *
import time
# Create a UDP socket   (IP protocl:  ipv4)
serverSocket = socket(AF_INET, SOCK_DGRAM)

IP_SERVER = '127.0.0.1'
PORT_SERVER = 3030
BUF_SIZE = 1024
time_out = 3
time_of_last_message = sys.maxsize
sequence_number_of_last = 0
# A match between the address and the port to the socket
serverSocket.bind((IP_SERVER, PORT_SERVER))
while True:
    message, address = serverSocket.recvfrom(BUF_SIZE)
    message = message.decode('ascii')
    arr = message.split(' ')
    data = arr[0]
    sequence_number = int(arr[1])
    time_of_send_message = arr[2]
    print(f'{message}')
    loss = ((sequence_number) - sequence_number_of_last) - 1
    sequence_number_of_last = sequence_number
    current_time = time.time()
    if (current_time - time_of_last_message > time_out):
        print('client disconnected ')
        sys.exit()
    time_of_last_message = current_time
    time_difference = current_time - float(time_of_send_message)
    message = f'{data.upper()} loss: {loss}    time difference: {time_difference}'
    rand = random.randint(0, 10)
    if rand >= 4:
        serverSocket.sendto(message.encode(), address)
