
#include <string>
#include <cstring>
#include <vector>
#include <set>
#include "Message.hpp"

uint32_t Message::count_message_id = 0;

Message make_connect(uint32_t source_id, uint32_t dest_id) {
  return Message(source_id, 0, CONNECT);
}
Message make_ack(uint32_t source_id, uint32_t dest_id, int message_id) {
  Message ack_message(source_id, dest_id, ACK);
  memcpy(ack_message.payload, &message_id, sizeof(message_id));
  return ack_message;
}
Message make_nack(uint32_t source_id, uint32_t dest_id, int message_id) {
  Message nack_message(source_id, dest_id, NACK);
  memcpy(nack_message.payload, &message_id, sizeof(message_id));
  return nack_message;
}
Message make_send(uint32_t source_id, uint32_t dest_id, uint32_t message_length ,const char *message, uint32_t trailing_msg,int num_of_message) {
  int pos_in_buffer = num_of_message - trailing_msg;
  Message send_message(source_id, dest_id, SEND, trailing_msg);
  memcpy(send_message.payload, &message_length, sizeof(message_length));
  strncpy(send_message.payload + fourBytes, message + (pos_in_buffer * size_of_data), message_length);
  return send_message;
}
Message make_discover(uint32_t source_id, uint32_t neighbour_id,uint32_t target,std::set<int> & visited_set ,int visited_size) {
  Message discover_message(source_id, neighbour_id, DISCOVER);
  memcpy(discover_message.payload, &target, sizeof(target));
  memcpy(discover_message.payload + fourBytes,&visited_size,fourBytes);
  std::copy(visited_set.begin(), visited_set.end(), discover_message.payload + eightBytes);
  return discover_message;
}
Message make_route(uint32_t source_id, uint32_t dest_id, uint32_t message_id ,uint32_t path_length, std::vector<uint> & path) {
  Message route_message(source_id, dest_id, ROUTE);
  memcpy(route_message.payload, &message_id, sizeof(message_id));
  memcpy(route_message.payload + fourBytes, &path_length, fourBytes);
  std::copy(path.begin(), path.end(), route_message.payload + eightBytes);
  return route_message;
}

Message make_relay(uint32_t source_id, uint32_t dest_id, uint32_t next, uint32_t length) {
  Message relay_message(source_id, dest_id, RELAY);
  relay_message.trailing_num = length;
  memcpy(relay_message.payload, &next, fourBytes);
  memcpy(relay_message.payload + fourBytes, &length, fourBytes);
  return relay_message;
}

int get_bytes_from (char * payload ,int start , int size) {
  int res;
  memcpy(&res,payload + start ,size);
  return res;
} 

std::vector <uint> get_path_from_payload (char* payload) {
        int path_length = get_bytes_from (payload,fourBytes,fourBytes);
        std::vector<uint> path (payload + eightBytes,payload + eightBytes + path_length);
         return path;
}
