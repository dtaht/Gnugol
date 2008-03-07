#ifndef gnugol_server_options_h
#define gnugol_server_options_h

struct server_options {
  int urls:1;
  int snippets:1;
  int ads:1;
  int titles:1;
  int misc:1;
  int reg:1;
  int reverse:1;
  int broadcast:1;
  int multicast:1;
  int force:1; 
  int secure:1;
  int xml:1;
  int html:1;
  int offline:1;
  int lucky:1;
  int prime:1;
  int cache:1;
  int engine:1;
  int mirror:1;
  int input:1;
  int plugin:1;
  int output:1;
  int blind:1;
  int verbose:1;
  int debug:1;
  int dummy:1;
  int ipv4:1;
  int ipv6:1;
  int trust:1;
  int dontfork:1;

  char *engine_name;
  char *outputfile;
  char *inputfile;
  char *pluginfile;
  char *language;
  char *server;
  char *client;
};

typedef struct server_options ServerOptions;

struct connections {
  int mcast[2];
  int local[2];
  int remote[2];
  int readfds[3];
  int writefds[3];
  int exceptfds[2];
  int nfds;
};

extern int server_process_options(int argc, char **argv, ServerOptions *o);

#endif
