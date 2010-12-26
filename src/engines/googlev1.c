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
#include "gnugol_engines.h"

#ifndef __GNUC__
#  define __attribute__(x)
#endif

#define TEMPLATE  "http://ajax.googleapis.com/ajax/services/search/web?v=1.0"
#define LICENSE_URL "I don't know if you can even get one anymore."
#define TOU "http://code.google.com/apis/websearch/terms.html"

/* See options at 
   http://code.google.com/apis/ajaxsearch/documentation/reference.html#_intro_fonje 
   including:
    rsz= large | small  (8 vs 4)
    start= <0-indexed offset>
    hl= <language of searcher>
    lr= <language of results>
    safe= active | moderate | off

*/

#if 0
static struct {
  int start;
  int rsz; // number of results
  int safe;
  char language[16];
  char ip[8*5+1]; // Room for ipv6 requests
} search_opt;
#endif

int GNUGOL_DECLARE_ENGINE(setup,google) (QueryOptions_t *q) {
  char   string[URL_SIZE];
  char   key   [256];
  size_t size;
  
  if(q->debug > 9) GNUGOL_OUTW(q,"Entering Setup\n");

  size = sizeof(key);
  if (gnugol_read_key(key,&size,".googlekey") != 0)
  {
    GNUGOL_OUTE(q,"A license key to search google is required. You can get one from: %s",LICENSE_URL);
    return -1;
  }

  if(q->nresults > 8) q->nresults = 8; // google enforces a maximum result of 8
  if(q->debug) GNUGOL_OUTW(q,"KEYWORDS = %s\n", q->keywords);
  size = snprintf(string,URL_SIZE,"%s&key=%s&rsz=%d&start=%d&q=%s",TEMPLATE,key,q->nresults,q->position,q->keywords); 
  
  if (size > sizeof(q->querystr))
  {
    GNUGOL_OUTE(q,"Size of URL exceeds space set aside for it");
    return -1;
  }
  
  strcpy(q->querystr,string);
  if(q->debug > 9) GNUGOL_OUTW(q,"Exiting Setup\n");
  return (int)size;
}

// turn quotes back into quotes and other utf-8 stuff
// FIXME: Error outs cause a memory leak from "root"
// use thread local storage? or malloc for the buffer
// FIXME: do fuller error checking 
//        Fuzz inputs!
// Maybe back off the number of results when we overflow the buffer
// This engine takes advantage (abuses!) the CPP pasting tokens
// with a couple macros to make the interface to json a 1 to 1 relationship 
// The code is delightfully short this way.

int GNUGOL_DECLARE_ENGINE(search,google) (QueryOptions_t *q) {
    unsigned int i;
    char *text;
    json_t *root,*responseData, *results;
    json_error_t error;
    if(q->debug) GNUGOL_OUTW(q,"%s: trying url: %s\n", q->engine_name, q->querystr); 

    text = jsonrequest(q->querystr);
    if(!text) {
      GNUGOL_OUTE(q,"url failed to work: %s", q->querystr); 
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

