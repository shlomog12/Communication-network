/*
Authors: Shlomo Glick, Dolev Abuhatzira
*/
#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include "Message.hpp"
#include "select.hpp"

class Node {

    public:
        struct Neighbour{
            uint32_t id;
            int socket;
            int port;
            char* ip;
        };

        struct Path_data {
             uint32_t path_length;
             std::vector<uint> path = {};
             int requests;
        };

    private:

        enum commands {e_setid,e_connect,e_send,e_route,e_peers,e_err};
        uint32_t id;
        int mysock;
        std::map <uint32_t ,struct Neighbour> connections;
        std::map <int , std:: queue <Message> > relays;
        std::map <int ,struct Path_data> paths;
        std::map <int ,struct Message> messages;

        const commands convert_to_enum(const std::string command);
        void disconnect(int file_descriptor);
        void get_commands_from_user(std::string command);
        void set_id(const std::string _id);
        void connect_from_user(const std::string ip_and_port);
        void send_from_user(const std::string _id,const std::string lenOfMessage, const std::string messgae);
        void print_route_to_node_id(const std::string _id);
        void print_neighbours();
        void send_ack(Message* message,int sock);
        void send_discovers(Message dis_msg, uint32_t target,std::set<int> & visited);
        Path_data update_path (std::vector<uint> & new_path , std::vector<uint> & current_path, uint32_t prev_message_id);
        void build_relays(std::vector<uint> & path, Message last_message);
        void send_route (Message discover_in_msg, std::vector<uint> & path);
        void print_path( std::vector<uint> & path);
        //from neighbour
        bool is_connect_message (int message_id);
        void get_message_from_nei(Message *message,int sock);
        void ack_from_nei(Message *ack , int socket);
        void nack_from_nei(Message *ack);
        void route_from_nei(Message* message , int socket);
        void send_from_nei(Message *message , int socket);
        void relay_from_nei(Message *message,int socket);
        void connect_from_nei(Message* message,int socket);
        void discover_from_nei(Message* message,int socket);
        void send_nack(uint32_t dest_id,uint32_t msg_id,int neighbour_socket);
        bool have_neighbours();
        bool is_neighbour(int id);
        int & get_socket(int id);
        std::vector<uint> & get_current_path(int discover_message_id) {return this->paths[discover_message_id].path;}
        void add_neighbour(uint32_t neighbour_id , int socket , int neighbour_port , char* ip);
        bool is_valid () {
            if (this->id == -1) { std::cout << "please insert id first" << std::endl; return false; }
            return true;
        }
    public:
        Node (uint16_t input_port) :id(-1) {
            mysock = socket(AF_INET,SOCK_STREAM,0);
            if (mysock < 0) {std::cout << "NACK" << std::endl; perror("failed to create a socket");} 
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY; //this macro init the default local ip
            address.sin_port = htons(input_port);
            if(bind(this->mysock,(struct sockaddr *) &address,sizeof(address)) < 0) perror("failed binding");
            if(listen(this->mysock,100) != 0) perror("failed to create listening socket");
        };
        int & get_socket() {return this->mysock;}
        void waiting();
        
};
