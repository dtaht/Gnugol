/* This engine implements a:
   gnugol -> google web api -> gnugol json translator engine
   using the google web api v1, which was deprecated Nov 1, 2010.
   According to google's API policy, they will no longer provide
   this api 3 years from that date. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <ctype.h>
#include <curl/curl.h>
#include "query.h"
#include "utf8.h"
#include "handy.h"
#include "formats.h"
#include "gnugol_engines.h"

#define TEMPLATE  "http://ajax.googleapis.com/ajax/services/search/web?v=1.0"
#define LICENSE_URL "I don't know if you can even get one anymore."
#define TOU "http://code.google.com/apis/websearch/terms.html"

const char description[] = "API is nearly feature complete, with support for multiple language input, output, safe search, position, and number of results.";

/* See options at
   http://code.google.com/apis/ajaxsearch/documentation/reference.html#_intro_fonje
   including:
    rsz= large | small  (8 vs 4)
    start= <0-indexed offset>
    hl= <language of searcher>
    lr= <language of results>
    safe= active | moderate | off

*/

/* Just plain overkill unless we start taking this as a string on
   the command line

static const gnugol_intmap_t safe_map[] = {
	{ 0, "off" },
	{ 1, "moderate" },
	{ 2, "active" },
	{-1, NULL }
};

*/
static const char *safe_map[] = {
	 "off", "moderate", "active", NULL
};

int setup(QueryOptions_t *q) {
  char   string[URL_SIZE];
  char   key   [256];
  size_t size;
  char   hl[8];
  char   lr[8];

  hl[0] = lr[0] = '\0';

  if(q->debug > 9) GNUGOL_OUTW(q,"Entering Setup\n");

  size = sizeof(key);
  if (gnugol_read_key(key,&size,".googlekey") != 0)
  {
    GNUGOL_OUTE(q,"A license key to search google is recommended. "
		  "You can get one from: %s",LICENSE_URL);
    size = 0;
  }

  if(q->safe < 0) q->safe = 0;
  if(q->safe > 2) q->safe = 2;
  if(q->debug > 5) GNUGOL_OUTW(q,"google: safesearch=%s\n",safe_map[q->safe]);
  if(q->nresults > 8) q->nresults = 8; // google enforced
  if(q->debug > 4) GNUGOL_OUTW(q,"google: KEYWORDS = %s\n", q->keywords);

/*
   While we can't be sure the user's two letter code is supported
   we CAN try harder to not mess up than this.
*/

  if((q->input_language[0] != '\0') &&
	  (isalpha(q->input_language[0])) &&
	  (isalpha(q->input_language[1]))) {
	  hl[0] = '&';
	  hl[1] = 'h';
	  hl[2] = 'l';
	  hl[3] = '=';
	  hl[4] = q->input_language[0];
	  hl[5] = q->input_language[1];
	  hl[6] = '\0';
  }
  if((q->output_language[0] != '\0') &&
	  (isalpha(q->output_language[0])) &&
	  (isalpha(q->output_language[1]))) {
	  lr[0] = '&';
	  lr[1] = 'l';
	  lr[2] = 'r';
	  lr[3] = '=';
	  lr[4] = q->output_language[0];
	  lr[5] = q->output_language[1];
	  lr[6] = '\0';
  }

  if (size == 0)
	  size = snprintf(string,URL_SIZE,
			  "%s&rsz=%d&start=%d&safe=%s%s%s&q=%s",
			  TEMPLATE,
			  q->nresults,q->position,
			  safe_map[q->safe], hl, lr, q->keywords);
  else
	  size = snprintf(string,URL_SIZE,
			  "%s&key=%s&rsz=%d&start=%d&safe=%s%s%s&q=%s",
			  TEMPLATE,key,
			  q->nresults,q->position,
			  safe_map[q->safe], hl, lr, q->keywords);

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

int search(QueryOptions_t *q) {
    unsigned int i;
    char *text;
    json_t *root,*responseData, *results;
    json_error_t error;
    if(q->debug) GNUGOL_OUTW(q,"%s: trying url: %s\n", q->engine_name, q->querystr);

    text = jsonrequest(q->querystr);
    if(!text) {
      GNUGOL_OUTE(q,"url failed to work: %s", q->querystr);
      return -1;
    }

    root = json_loads(text, &error);
    free(text);

    if(!root)
    {
        GNUGOL_OUTE(q,"error: on line %d: %s\n", error.line, error.text);
        return -1;
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
      gnugol_result_out(q,jsv(url),jsv(titleNoFormatting),jsv(content));
    }

    gnugol_footer_out(q);

    // FIXME: Go recursive if we overflowed the buffer

    json_decref(root);
    return 0;
}

