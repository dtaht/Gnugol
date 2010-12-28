/* This file implements a gnugol -> bing api -> gnugol json translator plugin 
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
#include "gnugol_engines.h"

#ifndef __GNUC__
#  define __attribute__(x)
#endif

#define TEMPLATE "http://api.bing.net/json.aspx?AppId=%s&Version=2.2&Market=%s&Query=%s&Sources=web&Web.Count=%d&JsonType=raw"
#define LICENSE_URL "http://www.bing.com/developers/createapp.aspx"
#define TOU "http://www.bing.com/developers/tou.aspx"

int GNUGOL_DECLARE_ENGINE(setup,bing) (QueryOptions_t *q) {
  char   string[URL_SIZE];
  char   key[256];
  size_t size = 0;
  char   uukeywords[512];

  size = sizeof(key);
  if (gnugol_read_key(key,&size,".bingkey") != 0)
  {
    GNUGOL_OUTE(q,"For bing, you need a license key from: %s\n",LICENSE_URL);
    return(-1);
  }
    
  if(q->nresults > 10) q->nresults = 10; // bing enforces a maximum result of 10, I think

  if (strlen(q->keywords) >= sizeof(uukeywords))
  {
    GNUGOL_OUTE(q,"Keywords exceed size of space set aside");
    return -1;
  }
  
  strcpy(uukeywords,q->keywords); // FIXME: convert to urlencoding
  size = snprintf(string,URL_SIZE,TEMPLATE,key,"en-US",uukeywords,q->nresults);
  strcpy(q->querystr,string);
  if(q->debug) GNUGOL_OUTW(q,"%s\n%s\n", q->keywords, string);
  return size;
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

int GNUGOL_DECLARE_ENGINE(search,bing) (QueryOptions_t *q) {
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

    root = json_loads(text, &error);
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

