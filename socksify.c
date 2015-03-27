//
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "connect_socks.h"
#include "parse_arg.h"
int main(int argc, char** argv) {

    if (argc!=4) {
        perror("Usage: socksify \"<command>\" socks_host socks_port\n");
        return 1;
    }
    set_socks_info(argv[2],argv[3]);
    printf("Socks info set: %s, %s.\n",socks_host,socks_port);
    char * arglist[256];
    char * envlist[256];
    char host[256];
    char port[256];
    snprintf(host,256,"SOCKS_HOST=%s",argv[2]);
    snprintf(port,256,"SOCKS_PORT=%s",argv[3]);
    envlist[0] = "LD_PRELOAD=./libmyconnect.so";
    envlist[1] = host;
    envlist[2] = port;
    envlist[3] = NULL;
    
    get_arglist(argv[1],arglist); 
    execvpe(get_cmd(argv[1]),arglist,envlist);
    printf("exec'd\n");
}

