
#include "Node.hpp"
#include "select.hpp"
#include <iostream>
using namespace std;

int main(int argc, char*  argv[]){
    uint16_t input_port;
    if (argc != 2){
        cout << "error, no port entered" << endl;
        return 0;
    }
    input_port = atoi(argv[1]);
    Node current (input_port); // create a node with new socket based on the input port
    add_fd_to_monitoring(current.get_socket()); // add the socket to the file descriptor set 
    while (true) {
        current.waiting();
    }
    return 0;
}
