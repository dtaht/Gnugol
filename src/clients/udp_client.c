#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>

#define DEBUG 1

#include "connect_client.h"
#include "port.h"
#include "query.h"

/* FIXME - figuring out calling malloc and free here is a bad thing 
   pass the buffer? */

int
query_main(char *query, QueryData *answers, char *host)
{
    int connfd, n, m, i;
    char *myhost;
    char *answer = (char *) malloc(1280);
    int cnt;

    if(host == NULL) {
      myhost = "localhost";
    }

    connfd = connect_client(myhost, QUERY_PORT, AF_UNSPEC, SOCK_DGRAM);

    if (connfd < 0) {
         fprintf(stderr,
                 "client error:: could not create connected socket "
                 "socket\n");
         return NULL;
    }

    m= write(connfd, query, strlen(query));
    memset(answer, 0, MAX_MTU);
    n = read(connfd,
             answer,
             MAX_MTU); // FIXME, leave running and timeout
    close(connfd);
    return(answer_parse(answer,answers));
}
