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

#ifndef __GNUC__
#define __attribute__(x)
#endif

int setup(
	QueryOptions_t *q 	__attribute__((unused)), 
	char           *string	__attribute__((unused)),
	size_t          lenstr	__attribute__((unused))
) 
{
  return 0;
}

// turn quotes back into quotes and other utf-8 stuff
// FIXME: Error outs cause a memory leak from "root"
// use thread local storage? or malloc for the buffer
// FIXME: do fuller error checking 
//        Fuzz inputs!
// Maybe back off the number of results when we overflow the buffer

int results(QueryOptions_t *q, char *urltxt,size_t urlsize __attribute__((unused))) {
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
