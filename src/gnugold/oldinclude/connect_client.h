#ifndef connect__client__h__
#define connect__client__h__

int
connect_client (const char *hostname,
                const char *service,
                int         family,
                int         socktype);
#endif
