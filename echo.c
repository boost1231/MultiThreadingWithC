//
//  echo.c
//  EchoServer
//
//  Created by Michael Delfanti on 9/7/15.
//  Copyright (c) 2015 Michael Delfanti. All rights reserved.
//

#include "echo.h"

void Echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
    {
        printf("server received %lu bytes\n", n);
        Rio_writen(connfd, buf, n);
    }
}