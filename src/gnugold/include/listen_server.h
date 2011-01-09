#ifndef listen__server__h__
#define listen__server__h__

/*
    listen_server
        creates a server server socket listening at a hostname:service
        using the family and socket type specified in the function
        arguments.
*/

int
listen_server(const char *hostname,
              const char *service,
              int         family,
              int         socktype);

#endif
