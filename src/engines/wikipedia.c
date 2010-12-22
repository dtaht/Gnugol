/* This file implements a gnugol -> wikipedia api -> gnugol json translator plugin */

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

/* 
From http://en.wikipedia.org/w/api.php

Parameters

Parameters:
  format         - The format of the output
                   One value: json, jsonfm, php, phpfm, wddx, wddxfm, xml, xmlfm, yaml, yamlfm, rawfm, txt, txtfm, dbg, dbgfm
                   Default: xmlfm
  action         - What action you would like to perform
                   One value: sitematrix, flagconfig, review, opensearch, articleassessment, stabilize, login, logout, query, expandtemplates, parse, feedwatchlist, help, paraminfo, purge, rollback, delete, undelete, protect, block, unblock, move, edit, upload, emailuser, watch, patrol, import, userrights
                   Default: help
  version        - When showing help, include version for each module
  maxlag         - Maximum lag
  smaxage        - Set the s-maxage header to this many seconds. Errors are never cached
                   Default: 0
  maxage         - Set the max-age header to this many seconds. Errors are never cached
                   Default: 0
  requestid      - Request ID to distinguish requests. This will just be output back to you


*/

/* Response

This module requires read rights.
Parameters:
  search         - Search string
  limit          - Maximum amount of results to return
                   No more than 100 (100 for bots) allowed.
                   Default: 10
  namespace      - Namespaces to search
                   Values (separate with '|'): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 100, 101, 108, 109
                   Default: 0
  suggest        - Do nothing if $wgEnableOpenSearchSuggest is false
  format         - 
Example:

Parameters:
  titles         - A list of titles to work on
  pageids        - A list of page IDs to work on
  revids         - A list of revision IDs to work on
  prop           - Which properties to get for the titles/revisions/pageids
                   Values (separate with '|'): info, revisions, links, langlinks, images, imageinfo, stashimageinfo, templates, categories, extlinks, categoryinfo, duplicatefiles, flagged, globalusage
  list           - Which lists to get
                   Values (separate with '|'): allimages, allpages, alllinks, allcategories, allusers, backlinks, blocks, categorymembers, deletedrevs, embeddedin, imageusage, logevents, recentchanges, search, tags, usercontribs, watchlist, watchlistraw, exturlusage, users, random, protectedtitles, oldreviewedpages, globalblocks, abuselog, abusefilters, articleassessment
  meta           - Which meta data to get about the site
                   Values (separate with '|'): siteinfo, userinfo, allmessages, globaluserinfo
  generator      - Use the output of a list as the input for other prop/list/meta items
                   NOTE: generator parameter names must be prefixed with a 'g', see examples.
                   One value: links, images, templates, categories, duplicatefiles, allimages, allpages, alllinks, allcategories, backlinks, categorymembers, embeddedin, imageusage, search, watchlist, watchlistraw, exturlusage, random, protectedtitles, oldreviewedpages
  redirects      - Automatically resolve redirects
  indexpageids   - Include an additional pageids section listing all returned page IDs.
  export         - Export the current revisions of all given or generated pages
  exportnowrap   - Return the export XML without wrapping it in an XML result (same format as Special:Export). Can only be used with export
Examples:
  api.php?action=query&prop=revisions&meta=siteinfo&titles=Main%20Page&rvprop=user|comment
  api.php?action=query&generator=allpages&gapprefix=API/&prop=revisions


 */


/* It's not clear to me if I have to login */

/* Want this in json format 

http://en.wikipedia.org/w/api.php?action=query&limit=4&format=json&prop=revisions&meta=siteinfo&titles=Main%20Page&rvprop=user|comment

http://en.wikipedia.org/w/api.php?action=query&limit=4&format=json&prop=info&titles=IPv6

*/

static int setup(QueryOptions_t *q, char *string) {
  char path[PATH_MAX];
  char key[256];
  int fd;
  int size = 0;
  snprintf(path,PATH_MAX,"%s/%s",getenv("HOME"), ".wikipediakey");
  if(fd = open(path,O_RDONLY)) {
    size = read(fd, key, 256);
    while(size > 0 && (key[size-1] == ' ' || key[size-1] == '\n')) size--;
    key[size] = '\0';
  }
  if(q->nresults > 100) q->nresults = 100; // wikipedia enforces a maximum result of 8

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

