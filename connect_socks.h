#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dlfcn.h>
#ifndef CONNECT_SOCKS_H
#define CONNECT_SOCKS_H
int (*og_socket)(int,int,int);
//og_socket =dlsym(RTLD_NEXT,"socket");
int (*og_connect)(int, const struct sockaddr *, socklen_t);
//og_connect=dlsym(RTLD_NEXT,"connect");
int *proxy_fd;
struct addrinfo *get_socks_addr(char *host, char *port);
int get_socks_fd(struct addrinfo *res);
int connect_socks(int sockfd, struct addrinfo *res);
int set_socks_addr(char *host, char *port);
char * socks_host;
char * socks_port;
void set_socks_info(char * host, char * port);
#endif
