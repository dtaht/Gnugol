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

char *
query_main(int argc,char *query, char *host)
{
    int connfd, n, m, i;
    char *myhost;
    char *answer = (char *) malloc(1280);
    int cnt;
    QueryData answers;

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
             sizeof(answer)); // FIXME, leave running and timeout
    close(connfd);
#if defined(DEBUG)
    printf(answer);
#endif
    cnt = answer_parse(answer, &answer); // Bless you David Rowe!
#if defined(DEBUG)
    printf(answer);
    printf(" cnt = %d\n",cnt);
#endif
    for (i = 0; i <= cnt; i++) {
      printf("<a href=%s>%s</a><br>", &answers.links[i], &answers.snippets[i]); 
    }
#if defined(DEBUG)
	printf("Answer: %s", answer);
#endif
    return (char *) answer;
}
