import sys
import time
from socket import *

IP_SERVER = '127.0.0.1'
PORT_SERVER = 3030
BUF_SIZE = 1024

MIN_RTT = sys.maxsize  # infinity
MAX_RTT = MIN_RTT * (-1)  # -(infinity)
NUMBER_LOSS = 0
sum = 0

# Create a UDP socket
clientSocket = socket(AF_INET, SOCK_DGRAM)
# Set a timeout value of 1 second
clientSocket.settimeout(1)
ServerAddress = (IP_SERVER, PORT_SERVER)

# Send ping 10 times
for i in range(1, 11):
    # Ping to server
    message = f'Ping {i} {time.time()}'
    start = time.time()
    clientSocket.sendto(message.encode(), ServerAddress)
    # time.sleep(3)
    try:
        data, serverAddress = clientSocket.recvfrom(BUF_SIZE)
        end = time.time()
        RTT = (end - start)
        sum = sum + RTT
        if RTT > MAX_RTT:
            MAX_RTT = RTT
        elif RTT < MIN_RTT:
            MIN_RTT = RTT
        data = data.decode('ascii')
        print(data)
    except timeout:
        NUMBER_LOSS += 1
        print('Request timed out')
    except ConnectionError:
        print('Connection Error')
        sys.exit()

recive_packets = (10 - NUMBER_LOSS)
average = (sum / recive_packets)
NUMBER_LOSS *= 10
print(f'min: {MIN_RTT}  max:  {MAX_RTT}  average: {average}  loss:{NUMBER_LOSS}%')
