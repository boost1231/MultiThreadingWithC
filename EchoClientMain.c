//
//  main.c
//  EchoClient
//
//  Created by Michael Delfanti on 9/7/15.
//  Copyright (c) 2015 Michael Delfanti. All rights reserved.
//


#include <stdio.h>
#include "csapp.h"

int main(int argc, const char **argv)
{
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;


    if (argc !=3)
    {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    host = argv[1];
    port = atoi(argv[2]);

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin))
    {
        Rio_writen(clientfd, buf, strlen(buf));
        Rio_readlineb(&rio, buf, MAXLINE);
        Fputs(buf, stdout);
    }

    Close(clientfd);
    exit(0);
}
