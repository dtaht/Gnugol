/* This engine implements dummy layer - useful for debugging the 
   formatting engine */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <curl/curl.h>
#include "query.h"
#include "utf8.h"
#include "handy.h"
#include "formats.h"

static struct {
  int start;
  int rsz; // number of results
  int safe;
  char language[16];
  char ip[8*5+1]; // Room for ipv6 requests
} search_opt;


int setup(QueryOptions_t *q, char *string,size_t lenstr) {
  return 0;
}

// turn quotes back into quotes and other utf-8 stuff
// FIXME: Error outs cause a memory leak from "root"
// use thread local storage? or malloc for the buffer
// FIXME: do fuller error checking 
//        Fuzz inputs!
// Maybe back off the number of results when we overflow the buffer

int results(QueryOptions_t *q, char *urltxt,size_t urlsize) {
    unsigned int i;
    char *text;
    char url[URL_SIZE];
    if(q->debug) GNUGOL_OUTE(q,"trying url: %s", urltxt); 
    gnugol_header_out(q);
    gnugol_result_out(q,"http://gnugol.taht.net","Gnugol - Command line Search","This is a dummy query",NULL);
    gnugol_result_out(q,"http://www.google.com","“Google Rocks” ","google is the source of many good things.",NULL);
    gnugol_result_out(q,"http://cryptolisting.taht.net","Cryptolisting - Making STARTTLS more common...  ","<b>This</b> has <i>html</i>.", NULL);
    gnugol_result_out(q,"http://www.taht.net","Dave's web site", "Dave has a web site?", NULL);

    gnugol_footer_out(q);

    // FIXME: Go recursive if we overflowed the buffer

    return 0;
}
