/* This file implements a gnugol -> google web api -> gnugol json translator plugin 
   using the google web api v1, deprecated Nov 1, 2010. */

/* Reminder, use thread local storage eventually */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <curl/curl.h>
#include "formats.h"
#include "query.h"
#include "utf8.h"
#include "handy.h"

#define TEMPLATE  "http://ajax.googleapis.com/ajax/services/search/web?v=1.0"
#define BUFFER_SIZE  (64 * 1024)  /* 64 KB */
#define URL_SIZE     (2048)
#define PATH_MAX 1024

#define STRIPHTML(a) strip_html(2048,a)

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


static int setup(QueryOptions *q, char *string) {
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
// use thread local storage or malloc for the buffer
// FIXME: do fuller error checking and use error buffers, too
// Maybe back off the number of results when we overflow the buffer
 
#define OUTPUTSIZE (64*1024)
#define OUTF(...) do { outsize += snprintf(&outbuf[outsize], OUTPUTSIZE-outsize, __VA_ARGS__); } while (0)

static int getresult(QueryOptions *q, char *urltxt) {
    unsigned int i;
    char *text;
    char url[URL_SIZE];
    json_t *root,*responseData, *results;
    json_error_t error;
    int outsize = 0;
    char outbuf[OUTPUTSIZE];
    if(q->debug) fprintf(stderr,"trying url: %s", urltxt); 

    text = jsonrequest(urltxt);
    if(!text) {
      fprintf(stderr,"url failed to work: %s", urltxt); 
      return 1;
    }

    root = json_loads(text, &error);
    free(text);

    if(!root)
    {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return 1;
    }
    
    GETOBJ(root,responseData);
    GETARRAY(responseData,results);  
    
    if(q->header) {
      char buffer[2048];
      strncpy(buffer,q->keywords,2048);
      STRIPHTML(buffer); // FIXME, need to convert % escapes to strings
      switch(q->format) {
	
      case FORMATELINKS: 
	OUTF( "<html><head><title>Search for: %s", buffer);
	OUTF( "</title></head><body>");
	break;
	
      case FORMATSSML: 
       	OUTF( "Result for <emphasis level='moderate'> %s </emphasis>\n", buffer); // FIXME keywords
	break;

      default: break;
      }
    }

    for(i = 0; i < json_array_size(results); i++)
    {
      json_t *result, *url, *titleNoFormatting, *content;
      const char *message_text;

      GETARRAYIDX(results,result,i);
      GETSTRING(result,url);
      GETSTRING(result,titleNoFormatting);
      GETSTRING(result,content);
      
      switch (q->format) {
	case FORMATWIKI: OUTF("[[%s|%s]] %s  \n",
				jsv(titleNoFormatting), 
				jsv(url), 
				jsv(content));  break;
	case FORMATSSML:  
	  { 
	    char tempstr[2048]; 
	    strcpy(tempstr,jsv(content));
	    STRIPHTML(tempstr);
	    OUTF("%s <mark name='%d'>%s</mark>.", tempstr, i+1, jsv(url)); 
	  }
	    break;
	case FORMATORG:  
	  { 
	    char tempstr[2048]; 
	    strcpy(tempstr,jsv(titleNoFormatting));
	    STRIPHTML(tempstr);
	    OUTF("\n** [[%s][%s]]\n", jsv(url), tempstr);
	    strcpy(tempstr,jsv(content));
	    STRIPHTML(tempstr);
	    OUTF("   %s", tempstr); 
	  }
	  break;
	case FORMATMDWN:  
	  { 
	    char tempstr[2048]; 
	    strcpy(tempstr,jsv(titleNoFormatting));
	    STRIPHTML(tempstr);
	    OUTF("\n[%s](%s)\n", jsv(url), tempstr);
	    strcpy(tempstr,jsv(content));
	    STRIPHTML(tempstr);
	    OUTF("   %s", tempstr); 
	  }
	  break;
	case FORMATTERM: 
	  { 
	    char tempstr[2048]; 
	    strcpy(tempstr,jsv(content));
	    STRIPHTML(tempstr);
	    OUTF("%s %s %s\n", jsv(url), 
		   jsv(titleNoFormatting), 
		   tempstr); 
	  }
	  break;

      case FORMATELINKS: OUTF("<p><a href=\"%s\">%s</a> %s</p>", 
				jsv(url), 
				jsv(titleNoFormatting), 
				jsv(content)); break;
	
      default: OUTF("<a href=\"%s\">%s</a> %s\n", 
		      jsv(url), jsv(titleNoFormatting), jsv(content)); 
      }
    }

    if(q->footer) {
      switch(q->format) {
      case FORMATELINKS: OUTF("</body></html>"); break;
      case FORMATORG:
      case FORMATMDWN:
      case FORMATTERM:   OUTF("\n"); break;
      default: break;
      }
    }

    json_decref(root);
    printf("%s",outbuf);
    return 0;
}

/* So, basically, inside of the results array, we want the url, title, and content for each */

// FIXME, add url encode
// FIXME UTF-8
// FIXME snprintf

int plugin_googlev1(QueryOptions *q) { 
  char basequery[URL_SIZE];
  char qstring[URL_SIZE]; 
  setup(q,basequery);
  getresult(q,basequery);
  return 0;
}
