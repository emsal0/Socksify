#define _GNU_SOURCE
#include "connect_socks.h"
#include <dlfcn.h>

int (*og_socket)(int domain,int type,int protocol);
og_socket=dlsym(RTLD_NEXT,"socket" );
int (*og_connect)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
og_connect = dlsym(RTLD_NEXT,"connect");
int socket(int domain, int type, int protocol) {
    if (proxy_fd != NULL) {
        return proxy_fd;
    } else {
        return (*og_socket)(domain,type,protocol);
    } 
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (proxy_fd != NULL) {
        char r_res[256],*req;
        req=r_res;
        *(req++)=5;
        *(req++)=1;
        *(req++)=0;
        *(req++)=1;
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr->ai_addr;
        struct in_addr *e_addr = &(ipv4->sin_addr); 
        memcpy(req,&(e_addr->s_addr),sizeof(*addr)); 
        req+=sizeof(*addr);

        unsigned short int sin_port = ipv4->sin_port; 
        *(req++) = sin_port >> 8;
        *(req++) = sin_port & 0xff;

        send(proxy_fd,r_res,req-r_res,0);
        recv(proxy_fd,r_res,10,0);
        return 0;
    } else {
        return (*og_connect)(sockfd, addr, addrlen);
    }
}
