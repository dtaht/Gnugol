#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>

#include "connect_client.h"
#include "port.h"

char answer[1280];
char *
query_main(int argc, char *query,char *host)
{
    int connfd, n, m;
    char *myhost;

    myhost = "localhost";
    if (argc == 2) 
        myhost=host;

    connfd = connect_client(myhost, QUERY_PORT, AF_UNSPEC, SOCK_DGRAM);

    if (connfd < 0) {
         fprintf(stderr,
                 "client error:: could not create connected socket "
                 "socket\n");
         return NULL;
    }

    m= write(connfd, query, strlen(query));

    memset(answer, 0, sizeof(answer));

    n = read(connfd,
             answer,
             sizeof(answer));

    close(connfd);
    printf("Answer: %s", answer);
    return (char *) answer;
}
