#ifndef SERVER_H
#define SERVER_H

#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

void server(uint16_t port, int backlog, size_t message_len);
void close_and_exit0(char *cause, char *message);
void close_and_exit1(char *cause, char *message, int sock_fd);
void close_and_exit2(char *cause, char *message, int sock_fd1, int sock_fd2);
void report_error(char *cause, char *message);

#endif
