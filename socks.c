#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char** argv) {

    //Tor daemon information	
    struct addrinfo hints, *res;
    int sockfd;
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo("localhost","9050",&hints,&res);

    //Create socket!
    sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    int connected = connect(sockfd,res->ai_addr,res->ai_addrlen);
    if (connected == -1) {
        perror("Error");
    }

    /* Initial connection request.
     * This uses the SOCKS 5 protocol defined in RFC 1928.
     * The initial request is composed of three bytes
     * msginit[0] is the version 
     * msginit[1] is the number of authentication methods the client (this program) is willing to supply
     * After that are bytes signifying the methods. The only one we're supplying is "No Auth", which is signified by 0
     */

    char buffer[256];
    char msginit[256];
    printf("%p",buffer);
    printf(" %p\n",msginit);
    *(msginit) = 5;
    *(msginit+1) = 1;
    *(msginit+2) = 0x00;
    send(sockfd, msginit, 3,0);
    printf("Sent.\n");
    recv(sockfd,buffer,2,0);	
    printf("%x, %x\n",buffer[0],buffer[1]);


    /* Now that we have recv()'d the confirmation message from the Tor daemon, the client is going to send a request
     * Spec is also defined in RFC 1928
     * A request looks like
     * -version number
     * -command (1 for connect, 2 for bind, 3 for UDP associate)
     * -reserved spot that's always 0
     * -type of IP address (1 for IPv4, 3 for Domain Name, 4 for IPv6)
     * -destination addr (number of bytes specified in the previous byte)
     * -destination port
     */

    char r_res[256], *req;
    req=r_res;
    *(req++) = 5;
    *(req++) = 1;
    *(req++) = 0;
    *(req++) = 1;	

    //get addrinfo for destination
    struct addrinfo dest_hints, *dest_res;
    memset(&dest_hints,0,sizeof dest_hints);
    dest_hints.ai_family = AF_UNSPEC;
    dest_hints.ai_socktype = SOCK_STREAM;
    getaddrinfo("duckduckgo.com",NULL,&dest_hints,&dest_res);
    char dest_str[INET6_ADDRSTRLEN];
    struct in_addr *addr;
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)dest_res->ai_addr;
    addr = &(ipv4->sin_addr);
    inet_ntop(dest_res->ai_family,addr,dest_str,sizeof dest_str);
    printf("%s\n",dest_str); //print the address

    //copy the addr into the request
    memcpy(req,&(addr->s_addr),sizeof(*addr));
    req+=sizeof(*addr);

    //hardcoded port 80 (yeah, I know)
    *(req++) = 0;
    *(req++) = 80;

    //send the request!
    send(sockfd,r_res,req-r_res,0);
    printf("Sent request.\n");
    recv(sockfd,r_res,10,0);

    //After the connection confirmation is received, everything sent to sockfd will send to the address you want it to!
}

