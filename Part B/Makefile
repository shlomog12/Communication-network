
all: client server client6 server6


server: Udp-Server.cpp
	g++ Udp-Server.cpp -o server


client: Udp-Client.cpp
	g++ Udp-Client.cpp -o client


server6: Udp-ServerSix.cpp
	g++ Udp-ServerSix.cpp -o server6


client6: Udp-ClientSix.cpp
	g++ Udp-ClientSix.cpp -o client6


clean:
	rm -f *.o client server



runS:
	./server



runC:
	./client



runS-strace:
	strace -f ./server



runC-strace:
	strace -f ./client



runC:

	./client



runS-strace:

	strace -f ./server



runC-strace:

	strace -f ./client
