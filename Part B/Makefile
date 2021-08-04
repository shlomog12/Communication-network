
all: client server client6 server6


server: Udp-Server.cpp
	g++ Udp-Server.cpp -o server


client: Udp-Client.cpp
	g++ Udp-Client.cpp -o client


server6: Udp-Server6.cpp
	g++ Udp-Server6.cpp -o server6


client6: Udp-Client6.cpp
	g++ Udp-Client6.cpp -o client6


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
