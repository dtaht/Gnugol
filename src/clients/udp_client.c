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
query_main(QueryData *answers, char *host)
{
    int connfd, n, m, i;
    char *myhost;
    char *answer = (char *) calloc(1280,1);
    char *query = answers->query;
    int cnt;

    if(host == NULL) {
      myhost = "2001:4f8:3:36:2e0:81ff:fe23:90d3"; // toutatis.taht.net
    } else {
      myhost = host;
    }
    if(answers->options.verbose) fprintf(stderr,"Connecting: %s\n",myhost);
    connfd = connect_client(myhost, QUERY_PORT, AF_UNSPEC, SOCK_DGRAM);

    if (connfd < 0) {
         fprintf(stderr,
                 "client error:: could not create connected socket "
                 "socket\n");
         return -1;
    }
    char *asdf = "GET LNK\nmangled querys suck\nEND\n";
    if(answers->options.verbose) fprintf(stderr,"Writing query: \"%s\" to socket of length %d\n", query, strlen(query));
//    m= write(connfd, query, strlen(query));
    m= write(connfd, asdf, strlen(asdf));
    memset(answer, 0, MAX_MTU);
    n = read(connfd,
             answer,
             MAX_MTU); // FIXME, leave running and timeout
    if(answers->options.verbose) fprintf(stderr,"Got response: %s\n", answer);
    close(connfd);
    strcpy(answers->query,answer); // ycuk
    return(answer_parse(answers));
}
