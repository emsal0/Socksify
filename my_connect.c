#define _GNU_SOURCE
#include "connect_socks.h"
#include <stdio.h>
#include <stdlib.h>
int socket(int domain, int type, int protocol) {
    if (len_proxy_fds == 65536) {
        exit(-1);
    }

    int (*og_socket)(int,int,int) = dlsym(RTLD_NEXT,"socket");
    int (*og_connect)(int, const struct sockaddr *, socklen_t) = dlsym(RTLD_NEXT,"connect");
    ssize_t (*og_send)(int,const void*,size_t,int) = dlsym(RTLD_NEXT,"send");
    og_socket = dlsym(RTLD_NEXT,"socket");
    og_connect = dlsym(RTLD_NEXT,"connect");
   
     

    if (type == SOCK_STREAM && domain == AF_INET) {

        struct addrinfo * socks_info = get_socks_addr("localhost","9050");
        int sockfd = get_socks_fd(socks_info);
        //printf("socket called: %d\n",sockfd);
                
        (*og_connect)(sockfd,socks_info->ai_addr,socks_info->ai_addrlen);
        char buffer[256];
        char *msginit = buffer;
        *(msginit++)=5;
        *(msginit++)=1;
        *(msginit++)=0;
        (*og_send)(sockfd,buffer,msginit-buffer,0);
        recv(sockfd,buffer,2,0);

        proxy_fds[len_proxy_fds++]=sockfd;

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
        //printf("connect called\n");
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

        (*og_send)(sockfd,r_res,req-r_res,0);
        int k;
        printf("r_res: ");
        for (k=0;k<req-r_res;k++) {
            printf("0x%x ",r_res[k]);
        }
        printf("\n");
        recv(sockfd,req_res,10,0);
        int j;
        for (j=0;j<10;j++) {
            printf("0x%x ", req_res[j]);
        }
        printf("\n");
        if (req_res[1] != 0) {
            printf("-1\n");
            return -1;
        }
        return 0;
    } else {
        return (*og_connect)(sockfd,addr,addrlen);
    }
}

/*ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    printf("send: %d\n",sockfd);
    ssize_t (*og_send)(int,const void*,size_t,int) = dlsym(RTLD_NEXT,"send");
    return (*og_send)(sockfd,buf,len,flags);
}*/
