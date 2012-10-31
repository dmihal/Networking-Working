#include "server.h"

const uint16_t PORT = 2012;
const int BACKLOG = 5;
const size_t MESSAGE_LEN = 1024;

int main(int argc, char **argv)
{
    server(argc >= 2 ? atoi(argv[1]) : PORT, BACKLOG, MESSAGE_LEN);
    return EXIT_SUCCESS;
}

void server(uint16_t port, int backlog, size_t message_len)
{
    char *message;
    struct addrinfo hints;
    char port_s[6];
    struct addrinfo *res;
    int gai_status;
    int welcome_fd;
    int conn_fd;
    ssize_t bytes;

    message = malloc(message_len);
    if (message == NULL) {
        close_and_exit0("malloc", NULL);
    }
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    snprintf(port_s, 6, "%d", port);
    gai_status = getaddrinfo(NULL, port_s, &hints, &res);
    if (gai_status != 0) {
        close_and_exit0("getaddrinfo", gai_strerror(gai_status));
    }
    welcome_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (welcome_fd == -1) {
        close_and_exit0("socket", NULL);
    }
    if (bind(welcome_fd, res->ai_addr, res->ai_addrlen) == -1) {
        close_and_exit1("bind", NULL, welcome_fd);
    }
    if (listen(welcome_fd, backlog) == -1) {
        close_and_exit1("listen", NULL, welcome_fd);
    }
    conn_fd = accept(welcome_fd, NULL, NULL);
    if (conn_fd == -1) {
        close_and_exit1("accept", NULL, welcome_fd);
    }
    bytes = recv(conn_fd, message, message_len, MSG_WAITALL);
    if (bytes != message_len) {
        close_and_exit2("recv",
                        bytes == -1 ? NULL : "unexpected message length",
                        conn_fd, welcome_fd);
    }
    printf("%s\n", message);
    bytes = send(conn_fd, message, message_len, 0);
    if (bytes != message_len) {
        close_and_exit2("send",
                        bytes == -1 ? NULL : "couldn't send all bytes",
                        conn_fd, welcome_fd);
    }
    if (close(conn_fd) == -1) {
        close_and_exit1("close", NULL, welcome_fd);
    }
    if (close(welcome_fd) == -1) {
        close_and_exit0("close", NULL);
    }
    freeaddrinfo(res);
    free(message);
}

void close_and_exit0(char *cause, char *message)
{
    report_error(cause, message);
    exit(EXIT_FAILURE);
}

void close_and_exit1(char *cause, char *message, int sock_fd)
{
    report_error(cause, message);
    if (close(sock_fd) == -1) {
        close_and_exit0("close", NULL);
    }
    exit(EXIT_FAILURE);
}

void close_and_exit2(char *cause, char *message, int sock_fd1, int sock_fd2)
{
    report_error(cause, message);
    if (close(sock_fd1) == -1) {
        close_and_exit1("close", NULL, sock_fd2);
    }
    if (close(sock_fd2) == -1) {
        close_and_exit0("close", NULL);
    }
    exit(EXIT_FAILURE);
}

void report_error(char *cause, char *message)
{
    if (message == NULL) {
        message = strerror(errno);
    }
    fprintf(stderr, "%s: %s\n", cause, message);
}
