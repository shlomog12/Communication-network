#ifndef __SELECT_H__
#define __SELECT_H__

int add_fd_to_monitoring(const unsigned int fd);
int wait_for_input();
void remove_fd_from_monitoring(const unsigned int fd);
#endif