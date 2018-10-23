/*
Copyright (C) 2011 Michael D. Täht

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

/*
<script>
  (function() {
    var cx = '012472956730922726645:gjqfh19akp4';
    var gcse = document.createElement('script');
    gcse.type = 'text/javascript';
    gcse.async = true;
    gcse.src = 'https://cse.google.com/cse.js?cx=' + cx;
    var s = document.getElementsByTagName('script')[0];
    s.parentNode.insertBefore(gcse, s);
  })();
</script>
<gcse:search></gcse:search>

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <curl/curl.h>
#include <iconv.h>
#include "query.h"
#include "utf8.h"
#include "handy.h"
#include "formats.h"
#include "gnugol_engines.h"

#define TEMPLATE  "https://www.googleapis.com/customsearch/v1?"
#define LICENSE_URL "https://developers.google.com/custom-search/v1/introduction"
#define TOU "After you get a key, it is free for 100 queries a day"

/* See options at
   http://code.google.com/apis/customsearch/v1/using_rest.html#response
   including:
    size= large | small  (8 vs 4)
    start= <0-indexed offset>
    hl= <language of searcher>
    lr= <language of results>
    safe= active | moderate | off

*/

static const char *safe_map[] = {
	 "off", "moderate", "active", NULL
};

int setup(QueryOptions_t *q) {
  char string[URL_SIZE];
  char key[256];
  size_t size = 0;
  char   hl[8];
  char   lr[8];

  hl[0] = lr[0] = '\0';

  if(q->debug > 9) GNUGOL_OUTW(q,"Entering Setup\n");

  size = sizeof(key);
  if (gnugol_read_key(key, &size,"googlekeyv2") != 0)
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
			  "%skey=%s&cx=012472956730922726645:gjqfh19akp4&q=%s",
			  TEMPLATE, key,
			  q->keywords);
  /*	  size = snprintf(string,URL_SIZE,
			  "%skey=%s&rsz=%d&start=%d&safe=%s%s%s&q=%s",
			  TEMPLATE, key,
			  q->nresults,q->position,
			  safe_map[q->safe], hl, lr, q->keywords);*/

  if (size > sizeof(q->querystr))
  {
    GNUGOL_OUTE(q,"Size of URL exceeds space set aside for it");
    return -1;
  }

  strcpy(q->querystr,string);
  if(q->debug > 9) GNUGOL_OUTW(q,"Exiting Setup\n");
  return (int)size;
}

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

/*

  if(size > 0) {
    snprintf(string,URL_SIZE-1,"%skey=%s&prettyprint=false&num=%d&start=%d&q=",TEMPLATE,key,q->nresults,q->position);
  } else {
    snprintf(string,URL_SIZE-1,"%sprettyprint=false&num=%d&start=%d&q=",TEMPLATE, q->nresults,q->position);
  }
  if(q->debug)       GNUGOL_OUTW(q,"Keywords: %s\n", q->keywords);
  strcat(string,q->keywords); // FIXME: convert to urlencoding
  return size;
}

*/

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

int search(QueryOptions_t *q) {
    unsigned int i;
    char *text;
    char url[URL_SIZE];
    json_t *root, *items, *handleResponse;
    json_error_t error;
    if(q->debug) GNUGOL_OUTW(q,"trying url: %s", q->querystr);

    text = jsonrequest(q->querystr);
    if(!text) {
      GNUGOL_OUTE(q,"url failed to work: %s", q->querystr);
      return 1;
    }

#ifdef HAVE_NEWJANSSON
    root = json_loads(text, 0, &error);
#else
    root = json_loads(text, &error);
#endif

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
      gnugol_result_out(q,jsv(link),jsv(title),jsv(snippet));
    }

    gnugol_footer_out(q);

    // FIXME: Go recursive if we overflowed the buffer

    json_decref(root);
    return 0;
}

