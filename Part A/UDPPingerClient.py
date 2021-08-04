import sys
import time
from socket import *

pings = 1
IP_SERVER='127.0.0.1'
PORT_SERVER = 4040
BUF_SIZE = 1024

MIN_RTT=sys.maxsize
MAX_RTT=MIN_RTT*-1
NUMBER_LOSS=0
average=0

# Create a UDP socket
clientSocket = socket(AF_INET, SOCK_DGRAM)
# Set a timeout value of 1 second
clientSocket.settimeout(1)
ServerAddress = (IP_SERVER, PORT_SERVER)

#Send ping 10 times 
for i in range(1,11):
    #Ping to server
    message = 'Ping'
    start = time.time()
    # Send ping
    # clientSocket.sendto(message, ServerAddress)
    clientSocket.sendto(message.encode(), ServerAddress)

    #If data is received back from server, print 
    try:
        data, serverAddress = clientSocket.recvfrom(BUF_SIZE)
        end = time.time()
        RTT=end-start
        average+=RTT
        if RTT>MAX_RTT: MAX_RTT=RTT
        elif RTT<MIN_RTT: MIN_RTT=RTT
        data=data.decode('ascii')
        print(f'{i}  {data}   {RTT}')
    #If data is not received back from server, print it has timed out  
    except timeout:
        NUMBER_LOSS+=1
        print ('Request timed out')
no_loss=10-NUMBER_LOSS
average/=no_loss
NUMBER_LOSS*=10
print(f'min: {MIN_RTT}  max:  {MAX_RTT}  average: {average}  loss:{NUMBER_LOSS}%')



# sock.sendto(MESSAGE.encode(), (UDP_IP, UDP_PORT))
# sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP, UDP_PORT))
