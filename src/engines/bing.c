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

#ifndef __GNUC__
#  define __attribute__(x)
#endif

#define TEMPLATE "http://api.bing.net/json.aspx?AppId=%s&Version=2.2&Market=%s&Query=%s&Sources=web&Web.Count=%d&JsonType=raw"
#define LICENSE_URL "http://www.bing.com/developers/createapp.aspx"
#define TOU "http://www.bing.com/developers/tou.aspx"

int setup(QueryOptions_t *q) {
  char string[2048];
  char path[PATH_MAX];
  char key[256];
  int fd;
  int size = 0;
  char uukeywords[512];

  snprintf(path,PATH_MAX,"%s/%s",getenv("HOME"), ".bingkey");
  if((fd = open(path,O_RDONLY))) {
    size = read(fd, key, 256);
    while(size > 0 && (key[size-1] == ' ' || key[size-1] == '\n')) size--;
    key[size] = '\0';
  }
  if(q->nresults > 10) q->nresults = 10; // bing enforces a maximum result of 10, I think

  if(size > 0) { 
    strcpy(uukeywords,q->keywords); // FIXME: convert to urlencoding
    size = snprintf(string,URL_SIZE-1,TEMPLATE,key,"en-US",uukeywords,q->nresults); 
  } else {
    GNUGOL_OUTE(q,"For bing, you need a license key from: %s\n",LICENSE_URL);
    return(-1);
  }
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

int getresult(QueryOptions_t *q, char *urltxt,size_t lenurl __attribute__ ((unused))) {
    unsigned int i;
    char *text;
    json_t *root,*Web, *SearchResponse, *Results;
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
      gnugol_result_out(q,jsv(Url),jsv(Title),jsv(Description),NULL);
    }

    gnugol_footer_out(q);

    // FIXME: Go recursive if we overflowed the buffer

    json_decref(root);
    return 0;
}

