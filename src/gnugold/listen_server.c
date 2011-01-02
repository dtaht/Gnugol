#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

//#include <b.h>
//#include <o.h>

#include "listen_server.h"

const int LISTEN_QUEUE=128;

int
listen_server(const char *hostname,
              const char *service,
              int         family,
              int         socktype)
{
  struct addrinfo hints, *res, *ressave;
  int n, sockfd;
  char host[1024];
  if(hostname == NULL) {
    char *h = getenv("GNUGOL_SERVER");
    if (h == NULL) {
#ifdef DUMMY_SERVER
      strcpy(host,"localhost");
      fprintf(stderr,"GNUGOL_SERVER should be set in your environment, using localhost\n");
#else
      fprintf(stderr,"GNUGOL_SERVER must be set in your environment\n");
#endif
    } else {
      strcpy(host,h);
    }
  } else {
    strcpy(host,hostname);
  }
  
  memset(&hints, 0, sizeof(struct addrinfo));

  /*
    AI_PASSIVE flag: the resulting address is used to bind
    to a socket for accepting incoming connections.
    So, when the hostname==NULL, getaddrinfo function will
    return one entry per allowed protocol family containing
    the unspecified address for that family.
  */

  hints.ai_flags    = AI_PASSIVE;
  hints.ai_family   = family;
  hints.ai_socktype = socktype;

  n = getaddrinfo(host, service, &hints, &res);

  if (n <0) {
    fprintf(stderr,
	    "getaddrinfo error:: [%s]\n",
	    gai_strerror(n));
    return -1;
  }

  ressave=res;

  /*
    Try open socket with each address getaddrinfo returned,
    until getting a valid listening socket.
  */
  sockfd=-1;
  while (res) {
    sockfd = socket(res->ai_family,
		    res->ai_socktype,
		    res->ai_protocol);

    if (!(sockfd < 0)) {
      if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
	break;

      close(sockfd);
      sockfd=-1;
    }
    res = res->ai_next;
  }

  if (sockfd < 0) {
    freeaddrinfo(ressave);
    fprintf(stderr,
	    "socket error:: could not open socket\n");
    return -1;
  }

  listen(sockfd, LISTEN_QUEUE);

  freeaddrinfo(ressave);

  return sockfd;
}
