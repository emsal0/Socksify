#define _GNU_SOURCE
#include "connect_socks.h"
#include <stdio.h>

int socket(int domain, int type, int protocol) {
    printf("socket called\n");
    int (*og_socket)(int,int,int) = dlsym(RTLD_NEXT,"socket");
    int (*og_connect)(int, const struct sockaddr *, socklen_t) = dlsym(RTLD_NEXT,"connect");
    if (type == SOCK_STREAM) {
        struct addrinfo * socks_info = get_socks_addr(socks_host,socks_port);
        int sockfd = get_socks_fd(socks_info);
        /*(*og_connect)(sockfd,socks_info->ai_addr,socks_info->ai_addrlen);
        char buffer[256];
        char *msginit = buffer;
        *(msginit++)=5;
        *(msginit++)=1;
        *(msginit++)=0;
        send(sockfd,buffer,msginit-buffer,0);
        recv(sockfd,buffer,2,0);*/
        return sockfd;
    } else {
        return (*og_socket)(domain,type,protocol);
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    printf("connect called\n");
    char r_res[256],*req;
    req=r_res;
    *(req++)=5;
    *(req++)=1;
    *(req++)=0;
    *(req++)=1;
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr;
    struct in_addr *e_addr = &(ipv4->sin_addr); 
    memcpy(req,&(e_addr->s_addr),sizeof(*addr)); 
    req+=sizeof(*addr);

    unsigned short int sin_port = ipv4->sin_port; 
    *(req++) = sin_port >> 8;
    *(req++) = sin_port & 0xff;

    send(sockfd,r_res,req-r_res,0);
    recv(sockfd,r_res,10,0);
    return 0;
}
