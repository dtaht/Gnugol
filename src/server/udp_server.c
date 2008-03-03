#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <time.h>
#include <malloc.h>
#include <sys/select.h>

#include "listen_server.h"
#include "port.h"
#include "query.h"
#include "plugins.h"

struct connections {
  int mcast[2];
  int local[2];
  int remote[2];
  int readfds[3];
  int writefds[3];
  int exceptfds[2];
  int nfds;
};

static int setup_connections(QueryOptions *o) {
  if (o->multicast) { }
  if (o->broadcast) { }
  if (o->cache) { }
  return 0;
}

/*
int get_it(QueryData *q, struct connections *c) {
  fd_set readfds;
  fd_set writefds;
  struct timespec timeout;
  sigset_t sigmask;
  timeout.tv_sec = 1;
  timeout.tv_nsec = 0;
  int ready = pselect(nfds, &c->readfds, &c->writefds, NULL,
                           timeout, &sigmask);
}
*/

int
main(int argc, char *argv[])
{
    int pdes[2];
    int listenfd, n;
    socklen_t addrlen;
    char *myhost;
    char answer[1280];
    struct sockaddr_storage clientaddr;
    time_t now;
    char b[1280];
    char clienthost[NI_MAXHOST];
    char clientservice[NI_MAXSERV];

    myhost=NULL;
    if (argc > 1)
        myhost=argv[1];
  
    QueryData *query = (QueryData *) calloc(sizeof(QueryData),1);
 
#ifdef DUMMY_SERVER
#else
    gnugol_plugin_gscrape_init();
#endif

    listenfd= listen_server(myhost, QUERY_PORT, AF_UNSPEC, SOCK_DGRAM);

    if (listenfd < 0) {
         fprintf(stderr,
                 "listen_server error:: could not create listening "
                 "socket\n");
         return -1;
    }
    memset(b, 0, sizeof(b));
    addrlen = sizeof(clientaddr);
    fprintf(stderr,"Waiting for a gnugol packet\n");

    for ( ; ;) {
        n = recvfrom(listenfd,
                     b,
                     sizeof(b),
                     0,
                     (struct sockaddr *)&clientaddr,
                     &addrlen);

        if (n < 0)
            continue;

        memset(clienthost, 0, sizeof(clienthost));
        memset(clientservice, 0, sizeof(clientservice));
        memset(query->answer, 0,1280);
	strcpy(query->query,query->answer);
	fprintf(stderr,"Got a packet\n");
#ifdef DUMMY_SERVER
	strcpy(query->answer,"LNK\nhttp://www.teklibre.com\nhttp://www.lwn.net\nhttp://www.slashdot.org\nhttp://a.very.busted.url\ngnugol://test+query\nEND\nSNP\nTeklibre is about to become the biggest albatross around David's head\nLWN ROCKS\nSlashdot Rules\nThis is a very busted url\nOne day we'll embed search right in the browser\nEND\n");
#else
        fprintf(stderr,"data packet to subprocess %s\n", query->query);
	gnugol_plugin_gscrape(query);
        fprintf(stderr,"data packet from subprocess %s\n", query->answer);
	
#endif

	// FIXME - COMPRESS THE OUTPUT, HASH THE DATA, ETC, ETC

        n = sendto(listenfd, query->answer, strlen(query->answer)+1, 0,
                   (struct sockaddr *)&clientaddr,
                   addrlen);

        getnameinfo((struct sockaddr *)&clientaddr, addrlen,
                    clienthost, sizeof(clienthost),
                    clientservice, sizeof(clientservice),
                    NI_NUMERICHOST);

        printf("Received request from host=[%s] port=[%s] string=%s\n",
               clienthost, clientservice,b);

        memset(b, 0, sizeof(b));

    }

    return 0;
}
