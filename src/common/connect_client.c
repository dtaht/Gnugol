#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

#include "connect_client.h"

int
connect_client (const char *hostname,
                const char *service,
                int         family,
                int         socktype)
{
    struct addrinfo hints, *res, *ressave;
    int n, sockfd;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = family;
    hints.ai_socktype = socktype;

    n = getaddrinfo(hostname, service, &hints, &res);

    if (n <0) {
        fprintf(stderr,
                "getaddrinfo error:: [%s]\n",
                gai_strerror(n));
        return -1;
    }

    ressave = res;

    sockfd=-1;
    while (res) {
        sockfd = socket(res->ai_family,
                        res->ai_socktype,
                        res->ai_protocol);

        if (!(sockfd < 0)) {
            if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
                break;

            close(sockfd);
            sockfd=-1;
        }
    res=res->ai_next;
    }

    freeaddrinfo(ressave);
    return sockfd;
}
