#ifdef WINDOWS
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <time.h>
#include <malloc.h>
#include <sys/select.h>
#endif

#include "listen_server.h"
#include "port.h"
#include "query.h"
#include "plugins.h"
#include "server.h"
// #include "ggol_multicast.h"

/* This code was written from looking at the multicast 
   code available in mdns-scan and mcs. 
   As such it shares all the flaws and most of the features.
*/

/*
    struct sockaddr_in sa;
    struct msghdr msg;
    struct iovec io;
    struct cmsghdr *cmsg;
    struct in_pktinfo *pkti;
    uint8_t cmsg_data[sizeof(struct cmsghdr) + sizeof(struct in_pktinfo)];
    int i, n;
    struct ifreq ifreq[32];
    struct ifconf ifconf;

    s_addr == INADDR_LOOPBACK ||
    s_addr == INADDR_ANY ||
    s_addr == INADDR_BROADCAST
*/


#define GWARN(...) fprintf(stderr,__VA_ARGS__);

#ifndef MCAST_JOIN_GROUP
#ifdef  IP_ADD_SOURCE_MEMBERSHIP


int ggol_join_ipv4_ssm(int socket, struct addrinfo * group, 
			 struct addrinfo * source, char * iface)
{
  struct	ip_mreq_source	mreq;

  if (iface != NULL)
    {
      /* See if interface is a literal IPv4 address */
      if ((mreq.imr_interface.s_addr = 
	   inet_addr(iface)) == INADDR_NONE)
	{
	  GWARN("Invalid interface address\n");
	  return -1;
	}
    }
  else
    {
      /* set the interface to the default */
      /* Arguably this should ignore tunnels and other dangerous devices */
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    }

  mreq.imr_multiaddr.s_addr = 
    ((struct sockaddr_in *)group->ai_addr)->sin_addr.s_addr;

  mreq.imr_sourceaddr.s_addr = 
    ((struct sockaddr_in *)source->ai_addr)->sin_addr.s_addr;
	
  /* Set the socket option */
  if (setsockopt(socket, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP,
		 (char *) &mreq, 
		 sizeof(mreq)) != 0)
    {
      GWARN("Could not join the multicast group: %s\n", 
		  strerror(errno));

      return -1;
    }
	
  return 0;
}
#else

int ggol_join_ipv4_ssm(int socket, struct addrinfo * group, 
                      struct addrinfo * source, char * iface)
{
  GWARN("Sorry, No support for IPv4 source-specific multicast in this build\n");
	
  return -1;
}
#endif

/* See http://www.faqs.org/rfcs/rfc3569.html
   for some details about source specific multicast 
   The address range 232/8
   has been assigned by IANA for SSM service in IPv4.  For IPv6, the
   range FF3x::/96 is defined for SSM services.
   My principal reason for looking at it is that this would allow a
   shared key by the clients, and crypto on the server.
*/

int ggol_join_ipv6_ssm(int socket, struct addrinfo * group, 
                      struct addrinfo * source, char * iface)
{
  GWARN("Sorry, No support for IPv6 source-specific multicast in this build\n");
	
  return -1;
}

#else /* if MCAST_JOIN_GROUP  .. */

#define ggol_join_ipv6_asm(a, b, c)      ggol_join_ip_asm((a), (b), (c))
#define ggol_join_ipv4_asm(a, b, c)      ggol_join_ip_asm((a), (b), (c))

int ggol_join_ip_asm(int socket, struct addrinfo * group, char * iface)
{
  struct	group_req	mreq;
  int			ip_proto;
  
  switch(group->ai_family) {
  case AF_INET6: ip_proto = IPPROTO_IPV6; break;
  case AF_INET: ip_proto= IPPROTO_IP; break;
  default: GWARN("bad protocol"); return -1;
  }

  // See RFC2533
  if (iface != NULL)
    {
      if ((mreq.gr_interface = if_nametoindex(iface)) 
	  == 0)
	{
	  GWARN("Ignoring unknown interface: %s\n", iface);
	}
      GWARN("Bringing up interface %s\n", iface);
    }
  else
    {
      mreq.gr_interface = 0;
    }		
			
  memcpy(&mreq.gr_group, group->ai_addr, group->ai_addrlen);

  /* Set the socket option */
  if (setsockopt(socket, ip_proto, MCAST_JOIN_GROUP, (char *)
		 &mreq, sizeof(mreq)) != 0)
    {
      GWARN("Could not join the multicast group: %s\n", 
		  strerror(errno));

      return -1;
    }
	
  return 0;
}

#endif /* MCAST_JOIN_GROUP */

#ifndef MCAST_JOIN_SOURCE_GROUP
int ggol_join_ipv4_asm(int socket, struct addrinfo * group, char * iface)
{
  struct	ip_mreq		mreq;
	
  if (iface != NULL)
    {
      /* See if interface is a literal IPv4 address */
      if ((mreq.imr_interface.s_addr = 
	   inet_addr(iface)) == INADDR_NONE)
	{
	  GWARN("Invalid interface address\n");
	  return -1;
	}
    }
  else
    {
      /* Set the interface to the default */
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    }

  mreq.imr_multiaddr.s_addr = 
    ((struct sockaddr_in *)group->ai_addr)->sin_addr.s_addr;

  /* Set the socket option */
  if (setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
		 (char *) &mreq, 
		 sizeof(mreq)) != 0)
    {
      GWARN("Could not join the multicast group: %s\n", 
		  strerror(errno));

      return -1;
    }
	
  return 0;
}

int ggol_join_ipv6_asm(int socket, struct addrinfo * group, char * iface)
{
  GWARN("Sorry, No support for IPv6 any-source multicast in this build\n");
	
  return -1;
}
#else /* if MCAST_JOIN_SOURCE_GROUP ... */

#define ggol_join_ipv4_ssm(a, b, c, d)   ggol_join_ip_ssm((a), (b), (c), (d))
#define ggol_join_ipv6_ssm(a, b, c, d)   ggol_join_ip_ssm((a), (b), (c), (d))

int ggol_join_ip_ssm(int socket, struct addrinfo * group, 
		    struct addrinfo * source,
		    char * iface)
{
  struct	group_source_req	mreq;
  int				ip_proto;

  switch(group->ai_family) {
  case AF_INET6: ip_proto = IPPROTO_IPV6;
  case AF_INET: ip_proto = IPPROTO_IP;
  default: GWARN("wrong ai family, continuing blindly");
  }

  if (iface != NULL)
    {
      if ((mreq.gsr_interface = if_nametoindex(iface)) 
	  == 0)
	{
	  GWARN("Ignoring unknown interface: %s\n", iface);
	}
    }
  else
    {
      mreq.gsr_interface = 0;
    }		
	
  memcpy(&mreq.gsr_group, group->ai_addr, group->ai_addrlen);
  memcpy(&mreq.gsr_source, source->ai_addr,source->ai_addrlen);

  /* Set the socket option */
  if (setsockopt(socket, ip_proto, MCAST_JOIN_SOURCE_GROUP, 
		 (char *) &mreq, 
		 sizeof(mreq)) != 0)
    {
      GWARN("Could not join the multicast group: %s\n", 
		  strerror(errno));

      return -1;
    }
	
  return 0;
}
#endif /* MCAST_JOIN_SOURCE_GROUP */

int multicast_setup_listen(int socket, struct addrinfo * group, 
			   struct addrinfo * source, char * iface)
{
  size_t rcvbuf;

#ifndef WINDOWS
  /* bind to the group address before anything */
  if (bind(socket, group->ai_addr, group->ai_addrlen) != 0)
    {
      GWARN("Could not bind to group-id\n");
      return -1;
    }
#else 
  if (group->ai_family == AF_INET)
    {
      struct sockaddr_in sin;

      sin.sin_family = group->ai_family;
      sin.sin_port = group->ai_port;
      sin.sin_addr = INADDR_ANY;

      if (bind(socket, (struct sockaddr *) sin, 
	       sizeof(sin)) != 0)
	{
	  GWARN("Could not bind to ::\n");
	  return -1;
	}
    }
  else if (group->ai_family == AF_INET6)
    {
      struct sockaddr_in6 sin6;

      sin6.sin6_family = group->ai_family;
      sin6.sin6_port = group->ai_port;
      sin6.sin6_addr = in6addr_any;

      if (bind(socket, (struct sockaddr *) sin6, 
	       sizeof(sin6)) != 0)
	{
	  GWARN("Could not bind to ::\n");
	  return -1;
	}
    }
#endif

  /* Set a receive buffer size of 16k */
  rcvbuf = 1 << 13;
  if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &rcvbuf, 
		 sizeof(rcvbuf)) < 0) {
    GWARN("Could not set receive buffer to 16k\n");
  }

  if (source != NULL)
    {
      switch(group->ai_family) {
      case AF_INET6: return ggol_join_ipv6_ssm(socket, group, source, iface);
      case AF_INET:  return ggol_join_ipv4_ssm(socket, group, source, iface);
      default: GWARN("Only IPv4 and IPv6 are supported\n");
	return -1;
      }
    }
  else
    {
      switch(group->ai_family) {
      case AF_INET6: return ggol_join_ipv6_asm(socket, group, iface);
      case AF_INET:  return ggol_join_ipv4_asm(socket, group, iface);
      default: GWARN("Only IPv4 and IPv6 are supported\n");
	return -1;
      }
      
    }
  /* We should never get here */
      return -1;
}
	

int multicast_setup_send(int socket, struct addrinfo * group, 
			 struct addrinfo * source)
{

  /* Anybody fooling with multicast should know that the signs
     ahead warn: 

     "DANGER: ANYBODY FOOLING WITH THIS SHOULD KNOW EXACTLY
      WHAT THEY ARE DOING. ABANDON ALL HOPE YE WHO ENTER."

      A default ttl of 2 limits the potential damage.
  */

  int	ttl	= 2; 
	
  if (source != NULL)
    {
      /* bind to the address before anything */
      if (bind(socket, source->ai_addr, source->ai_addrlen) != 0)
	{
	  GWARN("Could not bind to source-address\n");
	  return -1;
	}
    }

  if (group->ai_family == AF_INET)
    {
      if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *) 
		     &ttl, sizeof(ttl)) != 0)
	{
	  GWARN("Could not change the TTL\n");
	  // return -1; Probably not an error
	}
    }
  else if (group->ai_family == AF_INET6)
    {
      if (setsockopt(socket, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, 
		     (char *) &ttl, sizeof(ttl)) != 0)
	{
	  GWARN("Could not change the hop-count\n");
	  // return -1; Probably not an error
	}
    }

  return 0;
}

#ifdef TEST_MULTICAST
#include <stdlib.h>

main() {
  GWARN("Testing multicast\n");
  struct addrinfo addrs;
  struct sockaddr_in6 sock;
  
  // sin6.sin6_family = group->ai_family;
  // sin6.sin6_port = group->ai_port;
  // sin6.sin6_addr = in6addr_any;

  int fd = -1;
  int fd2;

  int err;
  struct addrinfo hints;
  struct addrinfo *rp;
  struct addrinfo *group, *rgroup;
  struct addrinfo *source, *rsource;
  int ttl = 2;

  memset(&hints, 0, sizeof(hints));

  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_NUMERICHOST || AI_PASSIVE || AI_NUMERICSERV;
  /* Get the group-id information */

  if ( (err =
	getaddrinfo(GNUGOL_IPV6_SMCAST_SEND, QUERY_PORT, &hints, &group)) != 0)
    {
      GWARN("Error getting group-id address: %s\n",
	    gai_strerror(err));
      exit(-1);
    }

  // gethostbyintuition();

  if ((err = getaddrinfo("::1", QUERY_PORT, &hints, &source)) != 0)
    {
      GWARN("Error getting my address: %s\n", gai_strerror(err));
      freeaddrinfo(group);
      exit(-1);
     }
  
  for (rp = group; rp != NULL; rp = rp->ai_next) {
    fd = socket(rp->ai_family, rp->ai_socktype,
		 rp->ai_protocol);
    if (fd == -1)
      continue;
    if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0)
      break;                  /* Success */
    
    close(fd);
  }
  
  if(fd < 1) {
    GWARN("Failed to get a socket\n");
  }

  if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, 
		 (char *) &ttl, sizeof(ttl)) != 0)
	{
	  GWARN("Could not change the hop-count\n");
	  // return -1; Probably not an error
	}
  
  /* 
     if ((sock = socket(AF_INET6, SOCK_DGRAM,HUH)) < 0)
     {
       GWARN("Could not create socket\n");
       freeaddrinfo(group);
       freeaddrinfo(source);
       exit(-1);
       } 
  */
//  if((fd2 = multicast_setup_send(fd, group, source)> 0))
if(1)
   {
     int i;
     /* Send 3 messages */
     for (i = 0; i < 3; i++) {
       write(fd,"Test MESSAGE\n",sizeof("Test MESSAGE\n")); // FIXME do this right
       sleep(10);
     }
     close(fd2);
   } else {
    GWARN("Couldn't setup multicast socket\n");
     }
  freeaddrinfo(group);
  freeaddrinfo(source);
}
  
#endif
