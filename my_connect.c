#define _GNU_SOURCE
//my_connect.c 
#include "connect_socks.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
int socket(int domain, int type, int protocol) {
    char * socks_h = getenv("SOCKS_HOST");
    char * socks_p = getenv("SOCKS_PORT");
    if (len_proxy_fds == 65536) {
        exit(-1);
    }

    int (*og_socket)(int,int,int) = dlsym(RTLD_NEXT,"socket");
    og_socket = dlsym(RTLD_NEXT,"socket");

    if ( (type == SOCK_STREAM) && (domain == AF_INET)) {
        struct addrinfo *socks_info = get_socks_addr(socks_h,socks_p);
        int sockfd = get_socks_fd(socks_info);
        
        proxy_fds[len_proxy_fds++]=sockfd;

        return sockfd;
    } else {
        return (*og_socket)(domain,type,protocol);
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int (*og_connect)(int, const struct sockaddr *, socklen_t) = dlsym(RTLD_NEXT,"connect");
    ssize_t (*og_send)(int,const void*,size_t,int) = dlsym(RTLD_NEXT,"send");
    char * socks_h = getenv("SOCKS_HOST");
    char * socks_p = getenv("SOCKS_PORT");
    int fd_proxy = 0;
    int i;
    for (i=0;i<len_proxy_fds;i++) {
        if (proxy_fds[i]==sockfd) {
            fd_proxy = 1;
        }
    }

    if (addr->sa_family == AF_INET && fd_proxy) {

        int opts;

        opts = fcntl(sockfd,F_GETFL);
        if (opts < 0) {
            perror("fcntl(F_GETFL");
            return -1;
        }

        int nonblocking;
        if ( (opts | O_NONBLOCK) == opts) {
            nonblocking = 1;
        } else {
            nonblocking = 0;
        }

        if (nonblocking) {
            fcntl(sockfd,F_SETFL,(opts ^ O_NONBLOCK)); 
        }
        struct addrinfo *socks_info = get_socks_addr(socks_h,socks_p);
        int connected = (*og_connect)(sockfd,socks_info->ai_addr,socks_info->ai_addrlen);
        if (connected!=0) {
            return -1;
        }
        char buffer[256];
        char *msginit = buffer;
        *(msginit++)=5;
        *(msginit++)=1;
        *(msginit++)=0;
        (*og_send)(sockfd,buffer,msginit-buffer,0);
        recv(sockfd,buffer,2,0);
        if (buffer[1] != 0) {
            return -1;
        }

        char r_res[256],*req;
        char req_res[10];
      
        req=r_res;
        *(req++)=5;
        *(req++)=1;
        *(req++)=0;
        *(req++)=1;
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr;
        struct in_addr *e_addr = &(ipv4->sin_addr); 
        memcpy(req,&(e_addr->s_addr),sizeof(*e_addr)); 
        req+=sizeof(*e_addr);

        unsigned short int sin_port = ipv4->sin_port; 
        *(req++) = sin_port & 0xff;
        *(req++) = sin_port >> 8;

        send(sockfd,r_res,req-r_res,0);
        recv(sockfd,req_res,10,0);
        if (nonblocking == 1) {
            fcntl(sockfd,F_SETFL,(opts | O_NONBLOCK));
        }
        if (req_res[1] != 0) {
            return -1;
        } else {
            return 0;
        }

    } else {
        return (*og_connect)(sockfd,addr,addrlen);
    }
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    ssize_t (*og_send)(int,const void*,size_t,int) = dlsym(RTLD_NEXT,"send");
    return (*og_send)(sockfd,buf,len,flags);
}
