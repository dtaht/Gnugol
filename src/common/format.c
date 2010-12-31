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

/*

Via:
http://www.w3.org/International/questions/qa-html-encoding-declarations

HTML5 <meta charset="UTF-8">
HTML4 <meta http-equiv="Content-type" content="text/html;charset=UTF-8">
XML   <?xml version="1.0" encoding="UTF-8"?>

meta_charset_map = {
{ FORMATHTML5, "<meta charset=\"UTF-8\">" },
{ FORMATHTML, "<meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\"> },
{ FORMATXML", "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" },
{ 0, NULL }

};

*/


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
      case FORMATHTML5:
	GNUGOL_OUTF(q, "<html><head><meta charset=\"UTF-8\"><title>%s%s</title></head><body>", NULLP(q->header_str), NULLP(buffer));
	break;
      case FORMATHTML:
      case FORMATELINKS:
	GNUGOL_OUTF(q, "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><title>%s%s</title></head><body>",NULLP(q->header_str), NULLP(buffer));
	break;
      case FORMATSSML:
       	GNUGOL_OUTF(q, "Result for <emphasis level='moderate'> %s </emphasis>\n", NULLP(buffer)); // FIXME keywords
	break;

      default: break;
      }
    }
    return(0);
}

int gnugol_footer_out(QueryOptions_t *q) {
  if(q->footer) {
    switch(q->format) {
    case FORMATHTML5:
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


static const char *levels[] = { "", "* ","** ","*** ","**** ","***** ", NULL };
static const char *hashes[] = { "", "# ","## ","### ","#### ","##### ", NULL };
static const char padding[] = "          ";

// FIXME: 8 possible combinations of output options here
// snippets/urls/titles

// Some notes here on the output logic
// with url "" and title non-"", we don't want [[]], just title
// with url non "" and title "", we want [[url]]
// with description non "" we want \ndescription\n (except when we only have an url?)
// with description "" we want nada on the previous stuff but a "\n"
// with "" "" "", don't output anything
/* stars
   url = 1
   title = 2
   snippet = 4
   spaces

   Grumble - c99 doesn't allow
	    printf("%2$*1$d", width, num);
*/

//   At least this one is complete

static const char *org_format_str[] = {
   "%s%s%s%s%s", // nothing
   "%s[[%s]]%s%s%s\n", // url only
   "%s%s[[%s]]%s%s\n", // title only
   "%s[[%s][%s]]%s%s\n", // url and snippet
   "%s%s%s%s%s\n", // snippet only
   "%s[[%s]]%s\n%s%s\n", // snippet and url
   "%s%s[[%s]]%s%s\n", // snippet and title
   "%s[[%s][%s]]\n%s%s\n", // everything
   NULL,
};

// FIXME: Haven't addressed anything but everything/nothing
//    GNUGOL_OUTF(q,"[[%s|%s]] %s  \n",t, u, s);

// How much do I care about the whole level thing for wikis?
// Check ikiwiki vs mediawiki formats

static const char *wiki_format_str[] = {
   "%s%s%s%s%s", // nothing
   "%s[[%s]]%s%s%s\n", // url only
   "%s%s[[%s]]%s%s\n", // title only
   "%s[[%s][%s]]%s%s\n", // url and snippet
   "%s%s%s%s%s\n", // snippet only
   "%s[[%s]]%s\n%s%s\n", // snippet and url
   "%s%s[[%s]]%s%s\n", // snippet and title
   "%s[[%s|%s]]\n%s%s\n", // everything
   NULL,
};

// FIXME: We could have a little fun with utf-8 here
// use “”

static const char *text_format_str[] = {
   "%s%s%s", // nothing
   "%s%s%s\n", // url only
   "%s%s%s\n", // title only
   "%s %s%s\n", // url and snippet
   "%s%s%s\n", // snippet only
   "%s %s%s\n", // snippet and url
   "%s%s %s\n", // snippet and title
   "%s %s\n%s\n", // everything
   NULL,
};

//    GNUGOL_OUTF(q,"<p><a href=\"%s\">%s</a> %s</p>", u, t, s);
//    FIXME: Actually implement levels? Nav?

static const char *html_format_str[] = {
   "%s%s%s", // nothing
   "<p><a href=\"%s\">See LINK</a>%s%s</p>", // url only
   "<p>%s%s%s</p>", // title only
   "<p><a href=\"%s\">%s</a>%s</p>", // url and snippet
   "<p>%s%s%s</p>", // snippet only
   "<p><a href=\"%s\">%s%s</a></p>", // snippet and url
   "<p>%s%s: %s</p>", // snippet and title
   "<p><a href=\"%s\">%s</a> %s</p>", // everything
   NULL,
};

// FIXME: Haven't addressed anything but everything/nothing

static const char *mdwn_format_str[] = {
   "%s%s%s%s%s", // nothing
   "%s%s(%s)%s%s\n", // url only
   "%s%s%s%s%s\n", // title only
   "%s%s(%s)%s%s\n", // url and snippet
   "%s%s%s%s%s\n", // snippet only
   "%s[%s]%s%s(%s)\n", // snippet and url
   "%s%s%s %s%s\n", // snippet and title
   "%s[%s](%s)\n%s%s\n", // everything
   NULL,
};

// FIXME: Haven't addressed anything but everything/nothing

static const char *textile_format_str[] = {
   "%s%s%s%s%s", // nothing
   "%s\"%s\":%s\n%s%s\n", // url only
   "%s\"%s\":%s\n%s%s\n", // title only
   "%s\"%s\":%s\n%s%s\n", // url and snippet
   "%s\"%s\":%s\n%s%s\n", // snippet only
   "%s\"%s\":%s\n%s%s\n", // snippet and url
   "%s\"%s\":%s\n%s%s\n", // snippet and title
   "%s\"%s\":%s\n%s%s\n", // everything
   NULL,
};

// FIXME: Have a format that makes sense to parse in LISP

#define NULLPINT(str,val) (str[0] == '\0' ? 0 : val)

/*

CLEANUP: Use C99 style dynamic arrays later

static int result_out(QueryOptions_t *q, const char *url,
                     const char *title,
                     const char *snippet) {
char u[strlen(url)];
char t[strlen(title)];
char s[strlen(snippet)];
strcpy(u,url);
STRIPHTML(t,title);
STRIPHTML(s,snippet);

printf("%s%s%s\n",u,s,t);

...

}

*/

// FIXME: This is going to take some serious testing
//        Human input forms are so messy
// FIXME: ORG: need to escape []

int gnugol_result_out(QueryOptions_t *q, const char *url, const char *title, const char *snippet) {
  char *t = NULLP(title);
  char *u = NULLP(url);
  char *s = NULLP(snippet);
  char *padstr;
  int offset = 0;
  int level = q->indent;
  if(level > 5) level = 5;
  if(level < 0) level = 2;

  char stripsnip[SNIPPETSIZE];
  char stripurl[URL_SIZE];
  char striptitle[URL_SIZE];

  if(!q->titles) t = "";
  if(!q->snippets) s = "";
  if(!q->urls) u = "";

  if(t[0] == '\0' && u[0] == '\0' && s[0] == '\0') {
  return(0);
  }

  if(t[0] == '\0' && u[0] == '\0') {
  padstr = "\0";
  } else {
  padstr = &padding[10-(level + 1)];
  }

  if(s[0] == '\0') padstr = "\0";

  offset = NULLPINT(u,1) | NULLPINT(t,2) | NULLPINT(s,4);
  q->returned_results++;

if(q->format != FORMATTERM) {
  strcpy(stripsnip,s);
  STRIPHTML(stripsnip);
} else {
  strcpy(stripsnip,s);
  STRIPHTML(stripsnip);
}
/* 
// NEW idea
// TERM output looks at the columns variable

// snippet = 0 - suppress
// snippet = 1 - do the right thing
// snippet = X - constrain size to that

// Unfortunately I don't know how to pass the
// following. Perhaps new a flag -B to break words?
// snippet = -1 - do the right thing, break words
// snippet = -X - break words at X

// to say this is currently messy would be an understatement

  int newsize = q->snippets;
  int size = strlen(s);
  if(size > 0) {
  char tempstr[size+1];  // FIXME: I'm not convinced this is enough
  strcpy(tempstr,s);
  size=strip_html(size,tempstr);
  if((q->format == FORMATTERM) && (size > 0)) {
     if(newsize == 1) {
     char *termsize = getenv("COLUMNS");
     if(termsize) size = strtoul(termsize, NULL, 10);
     } else {
     size = newsize;
     }
   }
   if(size-(level+1) > 0) {
   strncpy(stripsnip,s,u8_offset(tempstr,size));
   } else {
   stripsnip[0] = '\0';
   }
  } else {
   stripsnip[0] = '\0';
  }
}
 */

  strcpy(striptitle,t);
  STRIPHTML(striptitle);

  switch (q->format) {
  case FORMATIKI:
  case FORMATWIKI:
    GNUGOL_OUTF(q,wiki_format_str[offset],hashes[level],striptitle,u,padstr,stripsnip);
    break;
  case FORMATSSML: // FIXME: need to use marks properly
    {
     GNUGOL_OUTF(q,"%s <mark name='%d'>%s</mark>.", stripsnip, q->returned_results, u);
    }
    break;
  case FORMATORG:
    GNUGOL_OUTF(q,org_format_str[offset],levels[level],u,striptitle,padstr,stripsnip);
    break;
  case FORMATTEXTILE:
    GNUGOL_OUTF(q,textile_format_str[offset],padstr,striptitle,u,padstr,stripsnip);
    break;
  case FORMATMDWN:
    GNUGOL_OUTF(q,mdwn_format_str[offset],padstr,striptitle,u,padstr,stripsnip);
    break;
  case FORMATTERM:
  case FORMATTEXT:
    GNUGOL_OUTF(q,text_format_str[offset],u,striptitle,stripsnip);
    break;
  case FORMATHTML5:
  case FORMATHTML:
  case FORMATELINKS:
    GNUGOL_OUTF(q,html_format_str[offset],u,striptitle,stripsnip);
    break;
  case FORMATINFO:
  case FORMATCSV:
  case FORMATMAN:
  case FORMATXML:
  case FORMATSQL:
  case FORMATLISP:
    GNUGOL_OUTW(q,"format: Output format unsupported\n"); break;
  default:
    GNUGOL_OUTF(q,"<a href=\"%s\">%s</a> %s\n", u, t, s);
  }
return(0);
}
