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

// FIXME: Be more paranoid

int gnugol_init_QueryOptions(QueryOptions_t *q) {
  if(q) {
    memset(q,0,sizeof(QueryOptions_t));
    if((q->err.s = (char *) malloc(4096)) == NULL) q->err.size = 4096;
    if((q->out.s = (char *) malloc(1024*64)) == NULL) q->out.size = (1024*64);  
  }

}

int gnugol_free_QueryOptions(QueryOptions_t *q) {
  if(q) {
    if(q->err.s != NULL) { free(q->err.s); q->err.len = q->err.size = 0; }
    if(q->out.s != NULL) { free(q->out.s); q->out.len = q->out.size = 0; }
  }
  return(0);
}


int gnugol_header_out(QueryOptions_t *q) { 
    if(q->header) {
      char buffer[SNIPPETSIZE];
      strncpy(buffer,q->keywords,SNIPPETSIZE);
      STRIPHTML(buffer); // FIXME, need to convert % escapes to strings
      switch(q->format) {
	
      case FORMATELINKS: 
	GNUGOL_OUTF(q, "<html><head><title>Search for: %s", buffer);
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
    case FORMATELINKS: GNUGOL_OUTF(q,"</body></html>"); break;
    case FORMATORG:
    case FORMATMDWN:
    case FORMATTERM:   GNUGOL_OUTF(q,"\n"); break;
    default: break;
    }
  }
}

int gnugol_keywords_out(QueryOptions_t *q) {
  return(0);
}

int gnugol_result_out(QueryOptions_t *q, const char *url, const char *title, const char *snippet, const char *ad) {
  char tempstr[SNIPPETSIZE]; 
  switch (q->format) {
  case FORMATWIKI: 
    GNUGOL_OUTF(q,"[[%s|%s]] %s  \n",title, url, snippet);  
    break;
  case FORMATSSML:  
    { 
     strcpy(tempstr,snippet);
     STRIPHTML(tempstr);
     GNUGOL_OUTF(q,"%s <mark name='%d'>%s</mark>.", tempstr, 1, url);  // FIXME: sanely generate marks
    }
    break;
  case FORMATORG:  
    { 
      strcpy(tempstr,title);
      STRIPHTML(tempstr);
      GNUGOL_OUTF(q,"\n** [[%s][%s]]\n", url, tempstr);
      strcpy(tempstr,snippet);
      STRIPHTML(tempstr);
      GNUGOL_OUTF(q,"   %s", tempstr); 
    }
    break;
  case FORMATMDWN:  
    { 
      strcpy(tempstr,title);
      STRIPHTML(tempstr);
      GNUGOL_OUTF(q,"\n[%s](%s)\n", url, tempstr);
      strcpy(tempstr,snippet);
     STRIPHTML(tempstr);
     GNUGOL_OUTF(q,"   %s", tempstr); 
    }
    break;
  case FORMATTERM: 
    { 
      strcpy(tempstr,snippet);
      STRIPHTML(tempstr);
      GNUGOL_OUTF(q,"%s %s %s\n", url,title,tempstr); 
    }
    break;
    
  case FORMATELINKS: 
    GNUGOL_OUTF(q,"<p><a href=\"%s\">%s</a> %s</p>", url, title, snippet); 
    break;
    
  default: 
    GNUGOL_OUTF(q,"<a href=\"%s\">%s</a> %s\n", url, title, snippet); 
  }
return(0);
}
