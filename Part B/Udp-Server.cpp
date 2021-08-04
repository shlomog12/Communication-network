//
// Udp-Server.cpp Skeleton Ping-Pong.
// Robert Iakobashvili, BSD/MIT/Apache-license.
//
// 1. Correct the IP-address to be your local-IP address.
// 2. Compile using either MSVC or g++ compiler.
// 3. Run the executable.
// 4. In the console run netstat -a to see the UDP socket at port 5060.
// 5. Run the client and capture the communication using wireshark.
//
#include "stdio.h"
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 5060
// #define SERVER_PORT 999998



int main(){
	int s = -1;
	char buffer[80] = { '\0' };
	char message[] = "Hello, from the Server\n";
	int messageLen = strlen(message) + 1;

	// Create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){ //  -1 is SOCKET_ERROR
		printf("Could not create socket : %d", errno);
			return -1;
	}

	// setup Server address structure
	struct sockaddr_in serverAddress;
	memset((char *)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	if (inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &(serverAddress.sin_addr))<=0){
		printf("eror inett");
	};

	//Bind
	if (bind(s, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1){
		printf("bind() failed with error code : %d\n", errno);
		// TODO: cleanup the socket;
		return -1;
	}
	printf("After bind(). Waiting for clients");

	// setup Client address structure
	struct sockaddr_in clientAddress;
	

	socklen_t clientAddressLen = sizeof(clientAddress);

	memset((char *)&clientAddress, 0, sizeof(clientAddress));

	//keep listening for data
	while (1){
		fflush(stdout);

		// zero client address 
		memset((char *)&clientAddress, 0, sizeof(clientAddress));
		clientAddressLen = sizeof(clientAddress);

		//clear the buffer by filling null, it might have previously received data
		memset(buffer, '\0', sizeof (buffer));

		int recv_len = -1;

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buffer, sizeof(buffer) -1, 0, (struct sockaddr *) &clientAddress, &clientAddressLen)) == -1){
			printf("recvfrom() failed with error code : %d"	, errno);
			break;
		}

		char clientIPAddrReadable[32] = { '\0' };
		inet_ntop(AF_INET, &clientAddress.sin_addr, clientIPAddrReadable, sizeof(clientIPAddrReadable));

		//print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", clientIPAddrReadable, ntohs(clientAddress.sin_port));
		printf("Data is: %s\n", buffer);

		//now reply to the Client
		if (sendto(s, message, messageLen, 0, (struct sockaddr*) &clientAddress, clientAddressLen) == -1){
			printf("sendto() failed with error code : %d", errno);
			break;
		}
	}
	close(s);
    return 0;
}