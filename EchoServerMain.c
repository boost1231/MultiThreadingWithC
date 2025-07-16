#include "echo.h"
#include <sys/un.h>
#include <netinet/in.h>

int open_listenfd_from_book(int port);

int main(int argc, const char **argv)
{
    int listnenfd; // socket descriptor that the server will listen to requests on.
    int connfd;
    int port; // argv[1] will be converted to integer and assigned to this.
    socklen_t clientlen; // later on this will get the size of the client's socket address.

    struct sockaddr_in clientaddr;
        /*
         * the '_in' is for internet. All the properties start with sin. I think this can
         * be interpretted as socket internet. Remember the socket interface is designed to
         * work with any underlying protocols, not just tcp and ip. Thus the methods
         * of the sockets interface do not accespt a sockadr_in, but rather a generic socket
         * address structure called 'sockaddr'. See pg. 902 of my CSAPP book for more details.
         * See man 4 inet for the fields in sockaddr_in.
        */

    struct hostent *hp;
        /* This is what DNS uses to store host name and it's IP.
            char *h_name // Offical Domain Name of Host: e.g. www.espn.com
            char **h_aliases; // Null terminated array of domain names
            int h_addrtype; // Host address type (AF_INET: this is a constant that means Ipv 4 I believe)
            int h_length; // Length of ip address in bytes (4 for ipv4 I believe.)
            char **h_addr_list // null terminated array of in_addr structs.
         */

    char *haddrp;

    if(argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]); // I think argv[0] is the app name.
        exit(0);
    }

    port = atoi(argv[1]); // atoi converts a string to an integer

    listnenfd = open_listenfd_from_book(port);

    while(1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listnenfd, (SA *)&clientaddr, &clientlen);
            /*
                The Accept function is called to tell the server to wait for connection requests.
                It waits for the request to arrive on the listenfd descriptor which is bound to the
                servers socket addr (See open_listenfd_from_book function. It fills the clients socket address
                in clientaddr and returns connfd, a connection descriptor. Note, a listening descriptor
                serves as an endpoint for client connection requests. It is
                typically created once and exists for the lifetime of the server. Once a request is
                accepted, the connection descriptor is created for communication. It is created each time the server
                accepts a connection request and exists only as long as it takes the server to service the client. Remember,
                a connection is the pair of socket addresses, which are the endpoints of the connection. connfd is the endpoint
                on the server side. The server will use this descriptor to read stuff coming in from the client, and to
                write stuff back to the client.
             */

        hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
            // Remember this function looks up the DNS entry of the client.

        haddrp = inet_ntoa(clientaddr.sin_addr);
            // Convert the client IP to dotted decimal

        printf("server connected to %s (%s)\n", hp->h_name, haddrp);

        Echo(connfd);
            // This function sits in a loop and echos messages sent from the client to connfd.

        Close(connfd);
    }
}

// This functin is from my book.
int open_listenfd_from_book(int port)
{
    struct sockaddr_un unixaddr;
    struct sockaddr_in6 in6addr;
    int listenfd;
    int optval=1;

    struct sockaddr_in serveraddr;

    listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        // Creates a socket descriptor (used just like any other I/O descriptor).
        // socket creates an active socket, but it is converted later to a listening socekt as this is a server.

    if (listenfd < 0) return -1;

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(socklen_t)) < 0)
        return -1;
        // setSockOpt configures the server so that it can be terminated and restarted immediately.
        // By default a server will deny connection requests from clients for approximately 30 seconds.

    bzero((char *) &serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = PF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // This says requests to any IP of the server will be accepted
                                                    // Remember a server can have multiple network interfaces
                                                    // and each network interface has it's own IP.

    serveraddr.sin_port = htons((unsigned short)port);

    if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;
        /*
            bind tells the kernal to associate the socket address, serveraddr, with
            the socket descriptor, listenfd. SA is a type def for the struct sockaddr.
            This was a way to make the function signature not protocol specific. Remember
            these functions are part of the sockets interface which is not protocol
            specific. This program happens to be using the TCP/IP protocol.
         */

    if (listen(listenfd, LISTENQ) < 0)
        return -1;
        /*
            LISTENQ is 1024. This is the backlog parameter is a hint about the number of
            outstanding connection requests that the kernel should queue up before is starts to
            refuse requests (There is more to the exact meaning of this parameter). listen
            conversts listenfd from and active socket to a listening socket.
         */

    return listenfd;
}
