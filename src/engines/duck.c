/*
Copyright (C) 2014 Michael D. Täht

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see http://www.gnu.org/licenses/
 */

/* This engine implements a:
   gnugol -> duckduckgo web api -> gnugol json translator engine
   using the duckduckgo web api. */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <ctype.h>
#include <curl/curl.h>
#include <iconv.h>
#include "query.h"
#include "utf8.h"
#include "handy.h"
#include "formats.h"
#include "gnugol_engines.h"

//#define TEMPLATE  "https://duckduckgo.com/?t=gnugol&format=json&no_html=1&no_redirect"
#define TEMPLATE  "https://duckduckgo.com/?t=gnugol&format=json&no_html=1"
#define LICENSE_URL "https://duckduckgo.com/api"
#define TOU "https://duckduckgo.com/api"

static const char description[] = "API supports the limits of what duckduckgo makes available. No support for multiple language input, output, safe search, position, or number of results.";

/* See options at
   https://duckduckgo.com/api

   q: query
   format: output format (json or xml)
   If format=='json', you can also pass:
   callback: function to callback (JSONP format)
   pretty: 1 to make JSON look pretty (like JSONView for Chrome/Firefox)
   no_redirect: 1 to skip HTTP redirects (for !bang commands).
   no_html: 1 to remove HTML from text, e.g. bold and italics.
   skip_disambig: 1 to skip disambiguation (D) Type.
   kp=-1 unsafe, 1 safe
   kl=language
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
  //  char   key   [256];
  size_t size;
  //  char   hl[8];
  // char   lr[8];

  //  hl[0] = lr[0] = '\0';

  if(q->debug > 9) GNUGOL_OUTW(q,"Entering Setup\n");

  if(q->safe < 0) q->safe = 0;
  if(q->safe > 2) q->safe = 2;
  if(q->debug > 5) GNUGOL_OUTW(q,"duck: safesearch=%s\n",safe_map[q->safe]);
  if(q->nresults > 8) q->nresults = 8; // duck enforced
  if(q->debug > 4) GNUGOL_OUTW(q,"duck: KEYWORDS = %s\n", q->keywords);

/*
   While we can't be sure the user's two letter code is supported
   we CAN try harder to not mess up than this.
*/

/*  if((q->input_language[0] != '\0') &&
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

*/

/*

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

*/
	  size = snprintf(string,URL_SIZE,
			  "%s&q=%s",
			  TEMPLATE, q->keywords);


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
    char *stuff;
    json_t *root,*RelatedTopics, *results, *responseData;
    json_error_t error;
    if(q->debug) GNUGOL_OUTW(q,"%s: trying url: %s\n", q->engine_name, q->querystr);

    stuff = jsonrequest(q->querystr);
    if(!stuff) {
      GNUGOL_OUTE(q,"url failed to work: %s", q->querystr);
      return -1;
    }

#ifdef HAVE_NEWJANSSON
    root = json_loads(stuff, 0, &error);
#else
    root = json_loads(stuff, &error);
#endif

    free(stuff);

    if(!root)
    {
        GNUGOL_OUTE(q,"error: on line %d: %s\n", error.line, error.text);
        return -1;
    }

    GETARRAY(root,RelatedTopics);
    //        printf("%s\n",json_dumps(root,0));
    gnugol_header_out(q);

    for(i = 0; i < json_array_size(RelatedTopics); i++)
    {
      json_t *result, *FirstURL, *Text;
      GETARRAYIDX(RelatedTopics,result,i);
      GETSTRING(result,FirstURL);
      GETSTRING(result,Text);
      gnugol_result_out(q,jsv(FirstURL),jsv(Text),jsv(Text));
    }

    gnugol_footer_out(q);

    // FIXME: Go recursive if we overflowed the buffer

    json_decref(root);
    return 0;
}

