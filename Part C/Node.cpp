/*
Authors: Shlomo Glick, Dolev Abuhatzira
*/
#include<algorithm>
#include<vector>
#include<set>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h> 
#include "Node.hpp"
#include "Message.hpp"
#include <set>
using namespace std;

bool debugger = false;



void Node :: waiting(){ // this method running on while loop
    char buffer[SIZE_OF_BUFFER];

    int file_descriptor = wait_for_input();
    
    if (file_descriptor == 0) { // get input from stdin
        fgets(buffer, SIZE_OF_BUFFER, stdin);
        get_commands_from_user(buffer);
    }
    else if (file_descriptor == this->mysock) {
        int new_sock = accept(file_descriptor, NULL, NULL);
        if (new_sock < 0) perror("failed to accept");
        add_fd_to_monitoring(new_sock);
    }
    else {
        size_t len = recv(file_descriptor, buffer, sizeof(Message), 0);
         if (len == 0){
            disconnect(file_descriptor);
            return;
        }
        Message* message = (Message*)(buffer);
        get_message_from_nei(message, file_descriptor);
    }
}
vector<string> split (const string command, const char c){
    vector<string> choice;
    stringstream command_stream(command);
    string intermediate;
    while (getline(command_stream, intermediate, c)) {
        choice.push_back(intermediate);
    }
    return choice;
}
int create_socket(){
    int sock = 0;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) perror("failed to create a socket");
    return sock;
}
void Node::set_id(const string input_id){
    this->id = atoi(input_id.c_str());
    Message::count_message_id = (this->id * 100); //every message id have unique key (id00)
    cout << "ACK" << endl;
}
void Node::connect_from_user(const string input) {

    vector<string> v = split(input, ':');
    const char* neighbour_ip = v.at(0).c_str();
    uint32_t neighbour_port = atoi(v.at(1).c_str());

    struct sockaddr_in neighbour;
    neighbour.sin_family = AF_INET;
    neighbour.sin_port = htons(neighbour_port);
    neighbour.sin_addr.s_addr = inet_addr(neighbour_ip);
    int neighbour_socket = create_socket();

    if (connect(neighbour_socket, (struct sockaddr*)& neighbour, sizeof(neighbour)) == -1) { cout <<"NACK" << endl; perror("connection failed");}
    add_fd_to_monitoring(neighbour_socket);

    Message connect_message = make_connect(this->id, 0);
    if (send(neighbour_socket, &connect_message, sizeof(Message), 0) < 0) {cout << "NACK" << endl; perror("failed send");}
    messages[connect_message.id] = connect_message;
}
void Node::get_commands_from_user(string input) {
    vector<string> choice = split(input, ',');
    string function = choice.at(0);
    switch (convert_to_enum(function)) {
    case e_setid: 
        set_id(choice.at(1));
        break;
    case e_connect:
        if (is_valid()) connect_from_user(choice.at(1));
        break;
    case e_send: 
        if (is_valid()) send_from_user(choice.at(1), choice.at(2), choice.at(3));
        break;
    case e_route:
        if (is_valid()) print_route_to_node_id(choice.at(1));
        break;
    case e_peers: 
        if (is_valid()) print_neighbours();
        break;
    default:
        // cout << "Error: The command is invalid" << endl;
        cout << "NACK" << endl;
        break;
    }
}
void Node::get_message_from_nei(Message* message, int sock){
    int choice = message->function_id;

    switch (choice) {
    case ACK:
        if (is_valid()) ack_from_nei(message, sock);
        break;
    case NACK:
        if (is_valid()) nack_from_nei(message);
        break;
    case CONNECT:
        if (is_valid()) connect_from_nei(message, sock);
        break;
    case DISCOVER:
        if (is_valid()) discover_from_nei(message, sock);
        break;
    case ROUTE:
        if (is_valid()) route_from_nei(message, sock);
        break;
    case SEND:
        if (is_valid()) send_from_nei(message, sock);
        break;
    case RELAY:
        if (is_valid()) relay_from_nei(message, sock);
        break;
    default:
        // cout << "Error: The command is invalid" << endl;
        cout << "NACK" << endl;
        break;
    }
}
void Node::connect_from_nei(Message* connect_message, int sock){

    struct sockaddr_in address;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(sock, (struct sockaddr *)&address, &addr_size);
    uint32_t neighbour_port = ntohs(address.sin_port); // from internet to int order
    char *neighbour_ip = inet_ntoa(address.sin_addr);
    uint32_t neighbour_id = connect_message->source_id;

    add_neighbour(neighbour_id,sock,neighbour_port,neighbour_ip);

    cout << "CONNECT" << endl;
    cout << neighbour_id << endl;

    send_ack(connect_message, sock);
}
void Node::send_ack(Message* message, int socket){
    Message ack_message = make_ack(this->id, message->source_id, message->id);
    if (send(socket, &ack_message, sizeof(ack_message), 0) < 0)  perror("failed to send ack");
}
void Node::ack_from_nei(Message* ack, int socket){

    uint32_t neighbour_id = ack->source_id;
    std::cout << "get ack from " << neighbour_id << endl;

    if (!is_neighbour(neighbour_id)){

        struct sockaddr_in address;
        socklen_t addr_size = sizeof(struct sockaddr_in);
        int res = getpeername(socket, (struct sockaddr *)&address, &addr_size);
        uint32_t neighbour_port = ntohs(address.sin_port); // from internet to int order
        char *neighbour_ip = inet_ntoa(address.sin_addr);
        add_neighbour(neighbour_id,socket,neighbour_port,neighbour_ip);
      
    }
    cout << "ACK" << endl;
    cout << neighbour_id << endl;
}
void Node::send_from_user(const string id, const string data_length, const string message){
    if (!have_neighbours()) { cout << "NACK" << endl; return;}
    uint32_t dest_id = atoi(id.c_str());
    uint32_t len = atoi(data_length.c_str());
    const char* data = message.c_str();
    int32_t sequence_number = len / (size_of_data);
    int num_of_message = sequence_number;
    while (sequence_number >= 0) {
        uint32_t len_message;
        if (sequence_number == 0) len_message = (len % size_of_data);
        else len_message = size_of_data;
        Message send_message = make_send(this->id, dest_id, len_message, message.c_str(),sequence_number,num_of_message);        
        //now check if the destination node is neighbour of source node
        if (is_neighbour(dest_id)) {
            int neighbour_socket = get_socket(dest_id);
            if (send(neighbour_socket, &send_message, sizeof(Message), 0) < 0){cout << "NACK" << endl; perror("failed send");}
        }
        
        else {
            set<int> visited;
            visited.insert(this->id);
            send_discovers(send_message,dest_id,visited);
        }
        sequence_number--;
    }
}
void Node::discover_from_nei(Message* discover_message, int sock) {
    if (debugger) cout <<  "get discover from " << discover_message->source_id << endl; 
    uint32_t target = get_bytes_from(discover_message->payload,0,fourBytes);
    // check if the target is negihbour
    if (is_neighbour(target)) {
        vector<uint> path {target,this->id};
        send_route(*discover_message,path);
        return;
    }
    int visited_size = get_bytes_from (discover_message->payload,fourBytes,fourBytes);
    std::set<int> visited (discover_message->payload + eightBytes,discover_message->payload + eightBytes + (fourBytes * visited_size));
    visited.insert(this->id);
    send_discovers(*discover_message,target , visited);
}
void Node::send_discovers(Message comming_message, uint32_t target,set<int> & visited) {

    Path_data current_data {INT_MAX,{},0};   
    paths[comming_message.id] = current_data; 

    for (auto runner : connections) {

        Neighbour neighbour = runner.second;
        if (visited.count(neighbour.id)) continue;
        paths.at(comming_message.id).requests ++;
        int neighbour_socket = get_socket(neighbour.id);
        Message discover_message = make_discover(this->id, neighbour.id, target, visited, visited.size()); 
        if (debugger) cout <<  "send discover to " << neighbour.id << endl; 
        if (send(neighbour_socket, &discover_message, sizeof(Message), 0) < 0) {cout << "NACK" << endl; perror("failed send");};

        messages[discover_message.id] = comming_message;

    }

    if (paths[comming_message.id].requests == 0) { // its means that the current node is a leaf
        int neighbour_socket = connections.at(comming_message.source_id).socket;
        send_nack(comming_message.source_id,comming_message.id,neighbour_socket);
    }
}
void Node::route_from_nei(Message* route_msg, int socket){
    if (debugger) cout <<  "get route from " << route_msg->source_id << endl; 
    uint32_t out_message_id = get_bytes_from(route_msg->payload, 0, fourBytes);    
    Message prev_message = messages[out_message_id]; //extract the previous discover message

    vector<uint> path_from_nei = get_path_from_payload (route_msg->payload);
    path_from_nei.push_back(this->id);

    vector<uint> current_path = get_current_path (prev_message.id); 

    Path_data current_data = update_path (path_from_nei ,current_path, prev_message.id);
 
    if (current_data.requests == 0) {
        if (prev_message.source_id == this->id) {
                if (prev_message.get_type() == SEND) build_relays(current_data.path, prev_message);
                if (prev_message.get_type() == ROUTE) print_path(current_data.path);
        }
        else send_route(prev_message,current_data.path);
    }
}
void Node::build_relays(vector<uint> & path, Message last_message) {
    int len = (path.size() - 2);
    int neighbour_id = path.at(len);
    int neighbour_socket = get_socket(neighbour_id);
    for (int i = 1; i < len ; i++) {
        Message relay_message = make_relay(this->id, path.at(len), path.at(len - i), len - i);
        if (send(neighbour_socket, &relay_message, sizeof(relay_message), 0) < 0)  perror("failed send");
    }
    if (send(neighbour_socket, &last_message, sizeof(last_message), 0) < 0) perror("failed send");
}
void Node::relay_from_nei(Message* message, int socket){
    int key = message->source_id;
    if (!relays.count(key)) { // if relays doesnt contains the key we create one
        queue<Message> q;
        q.push(*message);
        relays[key] = q;
    }          
    else relays.at(key).push(*message);
    if (message->function_id == SEND) { // its means that is the last message that send
        Message head_queue = relays.at(key).front();
        relays.at(key).pop();
        if (head_queue.function_id == SEND) { // its means that is the last message that we have
            int target_socket = connections.at(head_queue.destination_id).socket;
            if (send(target_socket, &head_queue, sizeof(head_queue), 0) < 0) { perror("failed send");}
            messages[head_queue.id] = head_queue;
            return;
        }
        uint32_t next_id;
        memcpy(&next_id, head_queue.payload, fourBytes);
        int next_socket = connections.at(next_id).socket;
        while (!relays.at(key).empty()) {
            Message relay_message = relays.at(key).front();
            relays.at(key).pop();
            if (send(next_socket, &relay_message, sizeof(relay_message), 0) < 0) {perror("failed send");}
        }
    }
}
void Node:: send_route(Message discover_in_msg, vector<uint> & path){
    if (debugger) cout <<  "send route to " << discover_in_msg.source_id << endl; 
    Message route_message = make_route (this->id,discover_in_msg.source_id,discover_in_msg.id,path.size(),path);
    int neighbour_socket = get_socket(discover_in_msg.source_id);
    if (send(neighbour_socket, &route_message, sizeof(Message), 0) < 0) perror("failed send"); 
    // cout << "send route to:  " << discover_in_msg.source_id <<endl; 
}
void Node::print_route_to_node_id(const string id){
    uint32_t dest_id = atoi(id.c_str());
    if (this->id == dest_id) {cout << this->id << endl; return;}

    if(is_neighbour(dest_id)) {std::cout << this->id << "->" << dest_id << std::endl; return;}
    vector<uint> path = {};
    Message route_message = make_route(this->id, dest_id, -1, 0, path);
    set<int> visited;
    visited.insert(this->id);
    send_discovers(route_message, dest_id,visited);
}
void Node::print_path(std::vector<uint> & path){
    for (int i = path.size() - 1; i > 0; i --) {
        cout << path[i] << "->";
    }
    cout << path[0] << endl;
}
void Node::send_nack(uint32_t dest_id,uint32_t msg_id,int neighbour_socket){
    Message nack_message = make_nack(this->id,dest_id,msg_id);
    // cout << "send nack to:  " << dest_id <<endl; 
    if (send(neighbour_socket, &nack_message, sizeof(Message), 0) < 0) perror("failed send"); 
}
void Node::nack_from_nei(Message* nack) {
    uint32_t neighbour_id = nack->source_id;
    cout << "NACK" << endl;
    cout << neighbour_id << endl;
    uint32_t out_message_id = get_bytes_from(nack->payload, 0, fourBytes);    
    Message prev_message = messages[out_message_id];
    paths.at(prev_message.id).requests --;

    if (paths.at(prev_message.id).requests == 0) {
        if(paths.at(prev_message.id).path_length == INT_MAX) {
            if (this->id == prev_message.source_id) return;
            int neighbour_socket = connections.at(prev_message.source_id).socket;
            send_nack(prev_message.source_id,prev_message.id,neighbour_socket);
        }
        else {
            send_route(prev_message,paths.at(prev_message.id).path);
        }
    }
}
Node::Path_data Node::update_path(vector<uint> & new_path , vector<uint> & current_path, uint32_t prev_message_id) {
    paths[prev_message_id].requests --;
    
    if (current_path.size() == 0 || new_path.size() < current_path.size()) {    
        paths.at(prev_message_id).path = new_path;
        return paths.at(prev_message_id);
    }
    if (current_path.size() == new_path.size() && std::lexicographical_compare(new_path.begin(),new_path.end(),current_path.begin(),current_path.end())) {
        paths[prev_message_id].path = new_path;
        return paths.at(prev_message_id);
    }
    return paths.at(prev_message_id);
}
const Node::commands Node::convert_to_enum(const string command){
    if (command == "setid") return commands::e_setid;
    if (command == "connect") return commands::e_connect;
    if (command == "send") return commands::e_send;
    if (command == "route") return commands::e_route;
    if (command == "peers\n") return commands::e_peers;
    return commands::e_err;
}
void Node::add_neighbour(uint32_t neighbour_id , int socket , int neighbour_port , char * neighbour_ip) {
        Neighbour new_neighbour {neighbour_id,socket,neighbour_port,neighbour_ip};
        connections[neighbour_id] = new_neighbour;
}
bool Node::have_neighbours() {return (this->connections.size() > 0);}
bool Node::is_neighbour(int id) {return (connections.count(id) > 0);}
int & Node::get_socket(int id) {return this->connections[id].socket;}
void Node::send_from_nei(Message* message, int socket) {
    uint32_t neighbour_id = message->source_id;
    if (message->destination_id != this->id) {
        relay_from_nei(message, socket);
        return;
    }
    cout << "get send from id " << neighbour_id << " wich send : " << message->payload + fourBytes << endl;
    send_ack(message, socket);
}
void Node::print_neighbours(){
    for (auto nei : connections) {
        cout << nei.first << ",";
    }
    cout << endl;
}
void Node::disconnect(int file_descriptor){

    int id=-1;
    for(auto n:connections){
        if (n.second.socket == file_descriptor){
            id =n.second.id;
        }
    }
    if (debugger) cout << id << " disconnect" << endl; 
    this->connections.erase(id);
    remove_fd_from_monitoring(file_descriptor);
}

// setid,1
// connect,127.0.0.1:2002
