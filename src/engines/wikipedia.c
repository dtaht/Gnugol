/* This file implements a gnugol -> wikipedia api -> gnugol json translator plugin.
   It can be made general enough to support any mediawiki site. */

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

#define TEMPLATE  "http://en.wikipedia.org/w/api.php?action=query&prop=info&inprop=url&format=json&titles="

/* Via Mark Hershberger

The man most familiar with the API section of the code is Roan Kattouw
or, on IRC, Catrope (http://www.mediawiki.org/wiki/User:Catrope).  Keep
in mind that he is in the Netherlands, so he might be asleep when you
are awake.

But I've had a lot of experience with the API, and I don't mind
answering specific questions where I can.

This API query will give you everything but a “snippet”:

http://en.wikipedia.org/w/api.php?action=query&titles=Albert%20Einstein&prop=info&inprop=url&format=jsonfm

This will return the wikitext of the page:

http://en.wikipedia.org/w/api.php?action=query&titles=Albert%20Einstein&prop=revisions&rvprop=content&format=jsonfm

and this will return rendered content:

http://en.wikipedia.org/w/api.php?action=parse&page=Albert%20Einstein&format=jsonfm

You would need to change the format from “jsonfm” to “json” to get it in
actual JSON format.  The above query responses are html-ified so you can
see them in the browser.

fwiw, I found the above information on
<http://www.mediawiki.org/wiki/API:Query_-_Properties>
and <http://www.mediawiki.org/wiki/API:Parsing_wikitext>.

*/

static int setup(QueryOptions_t *q, char *string) {
  char path[PATH_MAX];
  char key[256];
  int fd;
  int size = 0;
  if(q->nresults > 10) q->nresults = 10; // wikipedia enforces a maximum result of ?
  // FIXME: Not clear to me yet how to limit the result size
  size = snprintf(string,URL_SIZE-1,"%s%s",TEMPLATE,q->keywords); // FIXME urlencode
  if(q->debug) GNUGOL_OUTW(q,"KEYWORDS = %s\n", q->keywords);
  return size;
}

// turn quotes back into quotes and other utf-8 stuff
// FIXME: do fuller error checking 
//        Fuzz inputs!
// Maybe back off the number of results when we overflow the buffer

static int getresult(QueryOptions_t *q, char *urltxt) {
    unsigned int i;
    char *text;
    char url[URL_SIZE];
    json_t *root,*responseData, *results;
    json_error_t error;
    if(q->debug) GNUGOL_OUTW(q,"trying url: %s", urltxt); 

    text = jsonrequest(urltxt);
    if(!text) {
      GNUGOL_OUTE(q,"url failed to work: %s", urltxt); 
      return 1;
    }

    root = json_loads(text, &error);
    free(text);

    if(!root)
    {
        GNUGOL_OUTE(q,"error: on line %d: %s\n", error.line, error.text);
        return 1;
    }
    
    GETOBJ(root,responseData);
    GETARRAY(responseData,results);  
    gnugol_header_out(q);

    for(i = 0; i < json_array_size(results); i++)
    {
      json_t *result, *url, *titleNoFormatting, *content;
      const char *message_text;
      GETARRAYIDX(results,result,i);
      GETSTRING(result,url);
      GETSTRING(result,titleNoFormatting);
      GETSTRING(result,content);
      gnugol_result_out(q,jsv(url),jsv(titleNoFormatting),jsv(content),NULL);
    }

    gnugol_footer_out(q);

    // FIXME: Go recursive if we overflowed the buffer

    json_decref(root);
    return 0;
}

// FIXME, add url encode
// FIXME UTF-8

int engine_wikipedia(QueryOptions_t *q) { 
  char basequery[URL_SIZE];
  char qstring[URL_SIZE]; 
  setup(q,basequery);
  getresult(q,basequery);
  return 0;
}

