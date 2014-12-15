#define _GNU_SOURCE
#include "connect_socks.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
int socket(int domain, int type, int protocol) {
    if (len_proxy_fds == 65536) {
        exit(-1);
    }

    int (*og_socket)(int,int,int) = dlsym(RTLD_NEXT,"socket");
    int (*og_connect)(int, const struct sockaddr *, socklen_t) = dlsym(RTLD_NEXT,"connect");
    ssize_t (*og_send)(int,const void*,size_t,int) = dlsym(RTLD_NEXT,"send");
    og_socket = dlsym(RTLD_NEXT,"socket");
    og_connect = dlsym(RTLD_NEXT,"connect");

    if ( (type == SOCK_STREAM) && (domain == AF_INET)) {
        /*
        struct addrinfo hints,*socks_info;
        memset(&hints,0,sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        int status;
        if (( status = getaddrinfo("localhost","9050",&hints,&socks_info) ) != 0) {
            printf("getaddrinfo status: %d\n",status);
        }
        */

        //printf("getaddrinfo status: %d\n",status);
        struct addrinfo *socks_info = get_socks_addr("127.0.0.1","9050");
        int sockfd = get_socks_fd(socks_info);
        //printf("socket called: %d\n",sockfd);
                
        
        proxy_fds[len_proxy_fds++]=sockfd;

        printf("sockfd: %d\n",sockfd);
        return sockfd;
    } else {
        //printf("socket called:%d, %d\n",domain,type);
        return (*og_socket)(domain,type,protocol);
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int (*og_connect)(int, const struct sockaddr *, socklen_t) = dlsym(RTLD_NEXT,"connect");
    ssize_t (*og_send)(int,const void*,size_t,int) = dlsym(RTLD_NEXT,"send");
    int fd_proxy = 0;
    int i;
    for (i=0;i<len_proxy_fds;i++) {
        if (proxy_fds[i]==sockfd) {
            fd_proxy = 1;
        }
    }

    printf("fd in proxy? %d\n",fd_proxy);
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

        fcntl(sockfd,F_SETFL,(opts & (~O_NONBLOCK))); //might change to | if it doesn't work

        //printf("connect called\n");
        struct addrinfo *socks_info = get_socks_addr("127.0.0.1","9050");
        int connected = (*og_connect)(sockfd,socks_info->ai_addr,socks_info->ai_addrlen);
        if (connected!=0) {
            printf("connect failed: %s\n",strerror(errno));
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
            printf("SOCKS handshake failed\n");
            return (*og_connect)(sockfd,addr,addrlen);
        }
        int i;

        printf("SOCKS handshake: ");
        for (i=0;i<2;i++) {
            printf("0x%x ",buffer[i]);
        }
        printf("\n");
        char r_res[256],*req;
        char req_res[256];
        
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
        int k;
        printf("SOCKS request: ");
        for (k=0;k<req-r_res;k++) {
            printf("0x%x ",r_res[k]);
        }
        printf("\n");
        recv(sockfd,req_res,10,0);
        int j;
        printf("SOCKS response to request: ");
        for (j=0;j<10;j++) {
            printf("0x%x ", req_res[j]);
        }
        printf("\n");
        if (req_res[1] != 0) {
            printf("-1\n");
            return -1;
        } else {
            return 0;
        }

        if (nonblocking == 1) {
            fcntl(sockfd,F_SETFL,(opts | O_NONBLOCK));
        } 
    } else {
        return (*og_connect)(sockfd,addr,addrlen);
    }
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    printf("send: %d, %s\n",sockfd,buf);
    ssize_t (*og_send)(int,const void*,size_t,int) = dlsym(RTLD_NEXT,"send");
    return (*og_send)(sockfd,buf,len,flags);
}
