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

/*
   This file implements a gnugol -> bing api -> gnugol json translator plugin
 */

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
#include "utf8_urlencode.h"

#define TEMPLATE "http://api.bing.net/json.aspx?AppId=%s&Version=2.2&Market=%s&Query=%s&Sources=web&Web.Count=%d&JsonType=raw"
#define LICENSE_URL "http://www.bing.com/developers/createapp.aspx"
#define TOU "http://www.bing.com/developers/tou.aspx"

/*
By default, the output has no newlines, and spaces are used between
array and object elements for a readable output. This behavior can be
altered by using the ``JSON_INDENT`` and ``JSON_COMPACT`` flags
described below. A newline is never appended to the end of the encoded
JSON data.*/

/*
.. function:: char *json_dumps(const json_t *root, size_t flags)

   Returns the JSON representation of *root* as a string, or *NULL* on
   error. *flags* is described above. The return value must be freed
   by the caller using :func:`free()`.

	  */


const char description[] = "API is pending support for multiple language input, and output, as well as safe search. Position, and number of results is supported.";

int setup(QueryOptions_t *q) {
  char   string[URL_SIZE];
  char   key[256];
  char hl[16];
  char lr[16];
  size_t size = 0;
  char   uukeywords[MAX_MTU];

  size = sizeof(key);
  if (gnugol_read_key(key,&size,"bingkey") != 0)
  {
    GNUGOL_OUTE(q,"For bing, you need a license key from: %s\n",LICENSE_URL);
    return(-1);
  }

  if(q->nresults > 10) q->nresults = 10; // bing enforces a maximum result of 10, I think

  if(q->safe < 0) q->safe = 0;
  if(q->safe > 2) q->safe = 2;

  if (strlen(q->keywords) >= sizeof(uukeywords))
  {
    GNUGOL_OUTE(q,"Keywords exceed size of space set aside");
    return -1;
  }

/*
   While we can't be sure the user's two letter country code is supported
   we CAN try harder to not mess up than this.
   FIXME: This is also incorrect for bing and is disabled
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

  if(!q->url_escape) {
	  url_escape_utf8(uukeywords,q->keywords);
  } else {
	  strcpy(uukeywords,q->keywords);
  }

  size = snprintf(string,URL_SIZE,TEMPLATE,key,"en-US",uukeywords,q->nresults);
  strcpy(q->querystr,string);
  if(q->debug) GNUGOL_OUTW(q,"%s\n%s\n", q->keywords, string);
  return size;
}

// turn quotes back into quotes and other utf-8 stuff
// FIXME: Error outs cause a memory leak from "root"?
// FIXME: do fuller error checking
//        Fuzz inputs!
// Maybe back off the number of results when we overflow the buffer
// This engine takes advantage (abuses!) the CPP pasting tokens
// with a couple macros to make the interface to json a 1 to 1 relationship
// The code is delightfully short this way.

int search(QueryOptions_t *q) {
    unsigned int i;
    char *text;
    json_t *root,*Web, *SearchResponse, *Results;
    json_error_t error;

    if(q->debug) GNUGOL_OUTW(q,"%s: trying url: %s\n", q->engine_name, q->querystr);

    text = jsonrequest(q->querystr);
    if(!text) {
      GNUGOL_OUTE(q,"url failed to work: %s\n", q->querystr);
      return 1;
    }

    if(q->debug) GNUGOL_OUTW(q,"%s: get url request succeeded: %s\n", q->engine_name, q->querystr);

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

    if(q->debug) GNUGOL_OUTW(q,"%s: got json url request!: %s\n", q->engine_name, q->querystr);
    GETOBJ(root,SearchResponse);
    GETOBJ(SearchResponse,Web);
    GETARRAY(Web,Results);
    gnugol_header_out(q);

    for(i = 0; i < json_array_size(Results); i++)
    {
      json_t *result, *Url, *Title, *Description;
      GETARRAYIDX(Results,result,i);
      GETSTRING(result,Url);
      GETSTRING(result,Title);
      GETSTRING(result,Description);
      gnugol_result_out(q,jsv(Url),jsv(Title),jsv(Description));
    }

    gnugol_footer_out(q);

    // FIXME: Go recursive if we overflowed the buffer

    json_decref(root);
    return 0;
}

