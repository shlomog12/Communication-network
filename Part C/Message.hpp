/*
Authors: Shlomo Glick, Dolev Abuhatzira
*/
#pragma once
#include <cstring>
#include <iostream>
#include <vector>
#include <set>

const unsigned int PAYLOAD_SIZE=492; 
const int ACK = 1;
const int NACK = 2;
const int CONNECT = 4;
const int DISCOVER = 8;
const int ROUTE = 16;
const int SEND = 32;
const int RELAY = 64; 
const int fourBytes = 4;
const int eightBytes = 8;
const uint32_t size_of_data = PAYLOAD_SIZE - fourBytes; // doesnt include the first four bytes (msg_id)


struct Message {

        static uint32_t count_message_id;
        uint32_t id;
        uint32_t source_id;
        uint32_t destination_id;
        uint32_t trailing_num;
        uint32_t function_id;
        char payload[PAYLOAD_SIZE]; 

        Message(){}; // default constructor
        
        Message(uint32_t src_id,uint32_t dest_id,uint32_t func_id,uint32_t trail_num = 0) { 
            id = count_message_id ++;
            source_id = src_id;
            destination_id = dest_id;
            function_id = func_id;
            trailing_num = 0;
            memset(payload,0,sizeof(payload));
        }
        int get_type() {return this->function_id;}
        
};
int get_bytes_from (char * payload ,int start , int size) ;
std::vector<uint> get_path_from_payload (char* payload);
Message make_connect(uint32_t source_id ,uint32_t dest_id);
Message make_ack(uint32_t source_id ,uint32_t dest_id,int message_id);
Message make_nack(uint32_t source_id ,uint32_t dest_id,int message_id);
Message make_send(uint32_t source_id,uint32_t dest_id,uint32_t len, const char* data_of_message,uint32_t trailing_msg,int num_of_message);
Message make_discover(uint32_t source_id, uint32_t neighbour_id,uint32_t target,std::set<int> & visited_array ,int sizeOfVisited);
Message make_route(uint32_t source_id, uint32_t dest_id, uint32_t message_id_,uint32_t path_length, std::vector<uint> & path);
Message make_relay(uint32_t source_id,uint32_t dest_id,uint32_t next ,uint32_t length);






