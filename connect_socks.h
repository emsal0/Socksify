#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#ifndef CONNECT_SOCKS_H
#define CONNECT_SOCKS_H
int *proxy_fd;
struct addrinfo *get_socks_addr(char *host, char *port);
int get_socks_fd(struct addrinfo *res);
int connect_socks(int sockfd, struct addrinfo *res);
#endif
