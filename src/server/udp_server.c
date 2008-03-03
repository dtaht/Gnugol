#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <time.h>

#include "listen_server.h"
#include "port.h"
#include "query.h"
#include "plugins.h"

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
   
#ifdef DUMMY_SERVER
#else
    gnugol_plugin_google_init(pdes);
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
        memset(answer, 0, sizeof(answer));

#ifdef DUMMY_SERVER
	strcpy(answer,"LNK\nhttp://www.teklibre.com\nhttp://www.lwn.net\nhttp://www.slashdot.org\nhttp://a.very.busted.url\ngnugol://test+query\nEND\nSNP\nTeklibre is about to become the biggest albatross around David's head\nLWN ROCKS\nSlashdot Rules\nThis is a very busted url\nOne day we'll embed search right in the browser\nEND\n");
#else
	// gnugol_plugin_google(&query,&answer,&formatter);
	gnugol_plugin_gscrape(&query,pdes);
#endif

	// FIXME - COMPRESS THE OUTPUT, HASH THE DATA, ETC, ETC

        n = sendto(listenfd, answer, strlen(answer)+1, 0,
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
