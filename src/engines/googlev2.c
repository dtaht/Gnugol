/* This engine implements a gnugol -> google web api -> gnugol json translator plugin 
   using the google web api v2 */

/* DISCLAIMER: I actually read the terms of service on this API, and I wish I didn't.
   It requires that I click on too many checkboxes to implement, and appears to
   be limited to only searching certain sites.

   It looks impossible to setup gnugol to "do the right thing" using this API.

   IANAL, but I believe that gnugol's usage would be covered under a DMCA exemption, 
   for purposes of "interoperability" with org, wiki, and speech synthesis. 

   It would be my hope that google would recognise this and grant me an exception 
   to this before I release the code in a finished version.

   Regardless, this API is 1/2 the speed of the other google API - and thus I 
   don't use it at present. I'm just keeping the code around if one day I figure
   out how to make it useful.

*/

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

#define TEMPLATE  "https://www.googleapis.com/customsearch/v1?"

/* See options at 
   http://code.google.com/apis/customsearch/v1/using_rest.html#response
   including:
    size= large | small  (8 vs 4)
    start= <0-indexed offset>
    hl= <language of searcher>
    lr= <language of results>
    safe= active | moderate | off

*/

static struct {
  int start;
  int rsz; // number of results
  int safe;
  char language[16];
  char ip[8*5+1]; // Room for ipv6 requests
} search_opt;


int GNUGOL_DECLARE_ENGINE(setup,googlev2) (QueryOptions_t *q) {
  char path[PATH_MAX];
  char key[256];
  int fd;
  int size = 0;
  snprintf(path,PATH_MAX,"%s/%s",getenv("HOME"), ".googlekeyv2");
  if(fd = open(path,O_RDONLY)) {
    size = read(fd, key, 256);
    while(size > 0 && (key[size-1] == ' ' || key[size-1] == '\n')) size--;
    key[size] = '\0';
  }
  if(q->nresults > 10) q->nresults = 10; // google enforces a maximum result of 10


/*

GET https://www.googleapis.com/customsearch/v1?key=INSERT-YOUR-KEY&cx=017576662512468239146:omuauf_lfve&q=lectures
    * Custom properties
          o cx: The identifier of the custom search engine.
          o cref: A URL pointing to the definition of a custom search engine.
          o safe: A description of the safe search level for filtering the returned results.
    * Custom query roles
          o nextPage: A role that indicates the query can be used to access the next logical page of results, if any.
          o previousPage: A role that indicates the query can be used to access the previous logical page of results, if any.


*/

/* WE WOULD NEED A CUSTOM SEARCH ENGINE HERE cx="bla" for this to work at all */

  if(size > 0) { 
    snprintf(string,URL_SIZE-1,"%skey=%s&prettyprint=false&num=%d&start=%d&q=",TEMPLATE,key,q->nresults,q->position); 
  } else {
    snprintf(string,URL_SIZE-1,"%sprettyprint=false&num=%d&start=%d&q=",TEMPLATE, q->nresults,q->position);
  }
  if(q->debug)       GNUGOL_OUTW(q,"Keywords: %s\n", q->keywords); 
  strcat(string,q->keywords); // FIXME: convert to urlencoding
  return size;
}

// turn quotes back into quotes and other utf-8 stuff
// FIXME: Error outs cause a memory leak from "root"
// use thread local storage? or malloc for the buffer
// FIXME: do fuller error checking 
//        Fuzz inputs!
// Maybe back off the number of results when we overflow the buffer

/*

 "items": [
 {
   "kind": "customsearch#result",
   "title": "Flower - Wikipedia, the free encyclopedia",
   "htmlTitle": "\u003cb\u003eFlower\u003c/b\u003e - Wikipedia, the free encyclopedia",
   "link": "http://en.wikipedia.org/wiki/Flower",
   "displayLink": "en.wikipedia.org",
   "snippet": "A flower, sometimes known as a bloom or blossom, is the reproductive structure found in flowering plants (plants of the division Magnoliophyta, ...",
  } 
*/

int GNUGOL_DECLARE_ENGINE(search,googlev2) (QueryOptions_t *q) {
    char *urltxt = q->querystr;
    unsigned int i;
    char *text;
    char url[URL_SIZE];
    json_t *root, *items, *handleResponse;
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
    
    GETOBJ(root,handleResponse);
    GETARRAY(handleResponse,items);  
    gnugol_header_out(q);

    for(i = 0; i < json_array_size(items); i++)
    {
      json_t *item, *link, *title, *snippet;
      const char *message_text;
      GETARRAYIDX(items,item,i);
      GETSTRING(item,link);
      GETSTRING(item,title);
      GETSTRING(item,snippet);
      gnugol_result_out(q,jsv(link),jsv(title),jsv(snippet),NULL);
    }

    gnugol_footer_out(q);

    // FIXME: Go recursive if we overflowed the buffer

    json_decref(root);
    return 0;
}

