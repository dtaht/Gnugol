#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

#define SERV_PORT 6000
#define LISTENQ 4
#define MAXLINE 256

void error(char *msg)
{
  perror(msg);
  exit(1);
}

/* Server.c */

struct addrinfo *
host_serv(const char *host, const char *serv, int family, int socktype);

int sctp_bind_arg_list(int sockfd, char **argv, int argc);

/* MAIN FUNCTION */

int main(int argc, char *argv[]) {
  int sockfd;
  int stream_no = 1;
  struct sockaddr_in servaddr;
  char sendline[]= "ping";
  
  //An SCTP one-to-many-style socket is created
  sockfd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sockfd < 0)
    error("ERROR opening socket");
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(argv[1]); // primary interface
  servaddr.sin_port = htons(SERV_PORT);

  if (bind(sockfd,(struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    error("ERROR on binding");

  if(argc >= 3)
    sctp_bind_arg_list(sockfd, argv, argc);

  //Enable incoming associations with the listen call
  if (listen(sockfd, LISTENQ) < 0 )
    error("ERROR listen()");

  /*The server sends a message "ping" until the client shutdown */
  while(1) {

    if(sctp_sendmsg(sockfd, sendline, strlen(sendline),
		    (struct sockaddr *) &servaddr, sizeof(servaddr),
		    0, 0, stream_no, 0, 0) < 0)
      error("ERROR, sctp_sendmsg()\n");
  }

  return (0);
} /* END MAIN FUNCTION */

struct addrinfo *
host_serv(const char *host, const char *serv, int family, int socktype)
{
  int n;
  struct addrinfo hints, *res;
  
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_CANONNAME; /* always return canonical name */
  hints.ai_family = family; /* AF_UNSPEC, AF_INET, AF_INET6 */
  hints.ai_socktype = socktype; /* 0, SOCK_STREAM, SOCK_DGRAM, etc */
  
  if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
    return (NULL);

  return (res); /* return pointer to first on linked list */
}

int sctp_bind_arg_list(int sockfd, char **argv, int argc)
{
  struct addrinfo *addr;
  char *bindbuf, *p, portbuf[10];
  int addrcnt = 0;
  int i;
  
//Allocating space for the bind arguments
  bindbuf = (char *) calloc((argc-2), sizeof(struct sockaddr_storage));
  p = bindbuf;

  //Set up the portbuf to be an ASCII representation of the port number
  sprintf(portbuf, "%d", SERV_PORT);

/* Pass each address and the port number to host_serv, along with
   AF_INET (ip4)
   and SOCK_SEQPACKET to specify that we are using SCTP */
  for (i = 0; i < (argc-2); i++ ) {
    addr = host_serv(argv[i+2], portbuf, AF_INET, SOCK_SEQPACKET);
    memcpy(p, addr->ai_addr, addr->ai_addrlen);
    freeaddrinfo(addr);
    addrcnt++;
    p +=addr->ai_addrlen;
  }

  if (sctp_bindx(sockfd, (struct sockaddr *) bindbuf,
		 addrcnt, SCTP_BINDX_ADD_ADDR) < 0)
    error("ERROR, sctp_bindx()");
  free(bindbuf);
  return (0);
}
