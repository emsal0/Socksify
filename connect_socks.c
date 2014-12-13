#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dlfcn.h>
int * proxy_fd = NULL;
char * socks_host;
char * socks_port;
int (*og_socket)(int,int,int);
int (*og_connect)(int, const struct sockaddr *, socklen_t);
void set_socks_info(char * host, char * port) {
    socks_host = host;
    socks_port = port;
}

struct addrinfo *get_socks_addr(char *host, char *port) {
    struct addrinfo hints,*res;
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(host,port,&hints,&res);
    return res;
}

int get_socks_fd(struct addrinfo *res) {
    og_socket = dlsym(RTLD_NEXT,"socket");
    int sockfd=(*og_socket)(res->ai_family,res->ai_socktype,res->ai_protocol);
    return sockfd;
}

int connect_socks(int sockfd, struct addrinfo *res) {
    og_connect = dlsym(RTLD_NEXT,"connect");
    return (*og_connect)(sockfd,res->ai_addr,res->ai_addrlen);
}
