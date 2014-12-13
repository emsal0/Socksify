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
    printf("Socks info set.\n");
    setenv("LD_PRELOAD","./libmyconnect.so",1);
    printf("LD_PRELOAD set.\n");
    char * arglist[256];
    get_arglist(argv[1],arglist); 
    execvp(get_cmd(argv[1]),arglist);
    printf("exec'd\n");
}

