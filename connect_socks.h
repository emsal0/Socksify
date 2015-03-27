#define _POSIX_C_SOURCE=200809L 
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dlfcn.h>
#ifndef CONNECT_SOCKS_H
#define CONNECT_SOCKS_H
int proxy_fds[256];
int len_proxy_fds;
struct addrinfo *get_socks_addr(char *host, char *port);
int get_socks_fd(struct addrinfo *res);
int connect_socks(int sockfd, struct addrinfo *res);
int set_socks_addr(char *host, char *port);
char * socks_host;
char * socks_port;
void set_socks_info(char * host, char * port);
#endif
