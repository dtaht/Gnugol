#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "query.h"
#include "formats.h"
#include "utf8.h"
#include "handy.h"

#ifndef __GNUC__
#  define __attribute__(x)
#endif

// FIXME: Be more paranoid

int gnugol_init_QueryOptions(QueryOptions_t *q) {
  if(q != NULL) {
    memset(q,0,sizeof(QueryOptions_t));
    if((q->err.s = (char *) malloc(4096)) != NULL) q->err.size = 4096;
    if((q->out.s = (char *) malloc(1024*64)) != NULL) q->out.size = (1024*64);  
    if((q->wrn.s = (char *) malloc(4096)) != NULL) q->wrn.size = (4096);  
  } else {
    return(1);
  }
  return(0);
}

int gnugol_reset_QueryOptions(QueryOptions_t *q) {
  if(q != NULL) {
    buffer_obj_t terr = q->err;
    buffer_obj_t twrn = q->wrn;
    buffer_obj_t tout = q->out;
    memset(q,0,sizeof(QueryOptions_t));
    q->err = terr;
    q->wrn = twrn;
    q->out = tout;
    q->err.len = q->out.len = q->wrn.len = 0;
 } else {
    return(1);
  }
  return(0);
}

int gnugol_free_QueryOptions(QueryOptions_t *q) {
  if(q != NULL) {
    if(q->err.s != NULL) { free(q->err.s); q->err.len = q->err.size = 0; }
    if(q->out.s != NULL) { free(q->out.s); q->out.len = q->out.size = 0; }
    if(q->wrn.s != NULL) { free(q->wrn.s); q->wrn.len = q->wrn.size = 0; }
  }
  return(0);
}


int gnugol_header_out(QueryOptions_t *q) { 
    if(q->header) {
      char buffer[SNIPPETSIZE];
      strncpy(buffer,q->keywords,SNIPPETSIZE);
      STRIPHTML(buffer); // FIXME, need to convert % escapes to strings
      switch(q->format) {
      case FORMATHTML:
      case FORMATELINKS: 
	GNUGOL_OUTF(q, "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><title>Search for: %s", buffer);
	GNUGOL_OUTF(q, "</title></head><body>");
	break;
	
      case FORMATSSML: 
       	GNUGOL_OUTF(q, "Result for <emphasis level='moderate'> %s </emphasis>\n", buffer); // FIXME keywords
	break;

      default: break;
      }
    }
    return(0);
}

int gnugol_footer_out(QueryOptions_t *q) {
  if(q->footer) {
    switch(q->format) {
    case FORMATHTML:
    case FORMATELINKS: GNUGOL_OUTF(q,"</body></html>"); break;
    case FORMATORG:
    case FORMATMDWN:
    case FORMATTERM:   GNUGOL_OUTF(q,"\n"); break;
    default: break;
    }
  }
  return 0;
}

int gnugol_keywords_out(QueryOptions_t *q __attribute__((unused))) {
  return(0);
}

static char *levels[] = { "", "*","**","***","****","*****", NULL };
static char padding[] = "          ";

// FIXME: 4! possible combinations of output options here 
// snippets/urls/titles/ads

int gnugol_result_out(QueryOptions_t *q, const char *url, const char *title, const char *snippet, const char *ad __attribute__((unused))) {
  char tempstr[SNIPPETSIZE]; 
  q->returned_results++;
  switch (q->format) {
  case FORMATWIKI: 
    GNUGOL_OUTF(q,"[[%s|%s]] %s  \n",title, url, snippet);  
    break;
  case FORMATSSML:  
    { 
     strcpy(tempstr,snippet);
     STRIPHTML(tempstr);
     GNUGOL_OUTF(q,"%s <mark name='%d'>%s</mark>.", tempstr, q->returned_results, url);
    }
    break;
  case FORMATORG:  
    { 
      int level = q->level;
      if(q->level > 5) level = 5;
      if(q->level < 0) level = 2;

      strcpy(tempstr,title);
      STRIPHTML(tempstr);
      if(level == 0) {
      GNUGOL_OUTF(q,"[[%s][%s]]\n", url, tempstr);
      } else {
      GNUGOL_OUTF(q,"%s [[%s][%s]]\n", levels[level], url, tempstr);
      }
      strcpy(tempstr,snippet);
      STRIPHTML(tempstr);
      if(level == 0) {
	GNUGOL_OUTF(q,"%s\n", tempstr); 
      } else {
	GNUGOL_OUTF(q,"%s%s\n", &padding[10-(level + 1)],tempstr); 
      }
    }
    break;
  case FORMATTEXTILE:  
    { 
      strcpy(tempstr,title);
      STRIPHTML(tempstr);
      GNUGOL_OUTF(q,"\"%s\":%s\n", tempstr, url);
      strcpy(tempstr,snippet);
      STRIPHTML(tempstr);
      GNUGOL_OUTF(q,"   %s\n", tempstr); 
    }
    break;

  case FORMATMDWN:  
    { 
      strcpy(tempstr,title);
      STRIPHTML(tempstr);
      GNUGOL_OUTF(q,"[%s](%s)\n", tempstr, url);
      strcpy(tempstr,snippet);
      STRIPHTML(tempstr);
      GNUGOL_OUTF(q,"   %s\n", tempstr); 
    }
    break;
  case FORMATTERM: 
    { 
      strcpy(tempstr,title);
      STRIPHTML(tempstr);
      GNUGOL_OUTF(q,"%s %s ", url, tempstr);
      strcpy(tempstr,snippet);
      STRIPHTML(tempstr);
      GNUGOL_OUTF(q,"%s\n", tempstr); 
    }
    break;
  case FORMATHTML:
  case FORMATELINKS: 
    GNUGOL_OUTF(q,"<p><a href=\"%s\">%s</a> %s</p>", url, title, snippet); 
    break;
    
  default: 
    GNUGOL_OUTF(q,"<a href=\"%s\">%s</a> %s\n", url, title, snippet); 
  }
return(0);
}
