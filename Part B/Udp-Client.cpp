//
// Udp-Client.cpp Skeleton Ping-Pong.
// Robert Iakobashvili for Ariel uni, BSD/MIT/Apache-license.
//
// 1. Correct the IP-address to be your local-IP address.
// 2. Check that server and client have the same rendezvous  port and the same IP-address
// 3. Compile using either MSVC or g++ compiler.
// 4. First run the server.
// 5. In the console run netstat -a to see the server UDP socket at port 5060.
// 6. Run the client and capture the communication using wireshark.
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
#include <iostream>
using namespace std;






#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 5060
// #define SERVER_PORT 999998

int main(){

	int s = -1;
	char bufferReply[80] = { '\0' };
	char message[] = "Good morning, Vietnam\n";
	int messageLen = strlen(message) + 1;

	// Create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){ //-1 is SOCKET_ERROR
		printf("Could not create socket : %d", errno);
		return -1;
	}

	// Setup the server address structure.
	// Port and IP should be filled in network byte order (learn bin-endian, little-endian)
	//
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	int rval = inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &serverAddress.sin_addr);
	if (rval <= 0){
		printf("inet_pton() failed");
		return -1;
	}

	//send the message
	if (sendto(s, message, messageLen, 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){
		printf("sendto() failed with error code  : %d", errno);
			return -1;
	}

	struct sockaddr_in fromAddress;
	socklen_t fromAddressSize = sizeof(fromAddress);



	memset((char *)&fromAddress, 0, sizeof(fromAddress));

	// try to receive some data, this is a blocking call
	if (recvfrom(s, bufferReply, sizeof(bufferReply) -1, 0, (struct sockaddr *) &fromAddress, &fromAddressSize) == -1){
		printf("recvfrom() failed with error code  : %d", errno);
		return -1;
	}
	cout << bufferReply << endl;
	// printf(bufferReply);
	close(s);
    return 0;
}