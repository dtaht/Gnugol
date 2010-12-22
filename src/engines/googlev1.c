/* This engine implements a gnugol -> google web api -> gnugol json translator plugin 
   using the google web api v1, which was depricated Nov 1, 2010. */

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

#define TEMPLATE  "http://ajax.googleapis.com/ajax/services/search/web?v=1.0"

/* See options at 
   http://code.google.com/apis/ajaxsearch/documentation/reference.html#_intro_fonje 
   including:
    rsz= large | small  (8 vs 4)
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


static int setup(QueryOptions_t *q, char *string) {
  char path[PATH_MAX];
  char key[256];
  int fd;
  int size = 0;
  snprintf(path,PATH_MAX,"%s/%s",getenv("HOME"), ".googlekey");
  if(fd = open(path,O_RDONLY)) {
    size = read(fd, key, 256);
    while(size > 0 && (key[size-1] == ' ' || key[size-1] == '\n')) size--;
    key[size] = '\0';
  }
  if(q->nresults > 8) q->nresults = 8; // google enforces a maximum result of 8

  if(size > 0) { 
    snprintf(string,URL_SIZE-1,"%s&key=%s&rsz=%d&start=%d&q=",TEMPLATE,key,q->nresults,q->position); 
  } else {
    snprintf(string,URL_SIZE-1,"%s&%d&%d&q=",TEMPLATE, q->nresults,q->position);
  }
  if(q->debug) printf("KEYWORDS = %s\n", q->keywords);
  strcat(string,q->keywords); // FIXME: convert to urlencoding
  return size;
}

// turn quotes back into quotes and other utf-8 stuff
// FIXME: Error outs cause a memory leak from "root"
// use thread local storage? or malloc for the buffer
// FIXME: do fuller error checking 
//        Fuzz inputs!
// Maybe back off the number of results when we overflow the buffer

static int getresult(QueryOptions_t *q, char *urltxt) {
    unsigned int i;
    char *text;
    char url[URL_SIZE];
    json_t *root,*responseData, *results;
    json_error_t error;
    if(q->debug) GNUGOL_OUTE(q,"trying url: %s", urltxt); 

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

int engine_googlev1(QueryOptions_t *q) { 
  char basequery[URL_SIZE];
  char qstring[URL_SIZE]; 
  setup(q,basequery);
  getresult(q,basequery);
  return 0;
}
