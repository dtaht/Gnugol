/* This code is Copyright (C) 2008 by Michael David Taht
   and released under the terms of the GNU AFFERO PUBLIC_LICENSE, version 3
   for details of this license see the COPYRIGHT file */

/* fastcgi client for gnugol */

#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include "query.h"
#include "formats.h"

// extern int engine_googlev2(QueryOptions_t *q);
extern int engine_bing(QueryOptions_t *q);
extern int engine_googlev1(QueryOptions_t *q);
extern int engine_dummy(QueryOptions_t *q);

struct  output_types {
	int id;
	char *desc;
};


// FIXME: Verify differences between ikiwiki and media wiki format

static struct output_types output_type[] = {
	{ FORMATHTML, "html" },
	{ FORMATWIKI, "wiki" },
	{ FORMATXML,  "xml" },
	{ FORMATJSON, "json" },
	{ FORMATORG,  "org" },
	{ FORMATTERM, "text" },
	{ FORMATSSML, "ssml" },
	{ FORMATMDWN, "mdwn" },
	{ 0, NULL },
};


#define penabled(a) if(o->a) fprintf(fp,"" # a " ");

int
print_enabled_options(QueryOptions_t *o, FILE *fp) {
  if(o->verbose) fprintf(fp,"Search Keywords: %s\n",o->keywords);
  fprintf(fp,"Results Requested: %d\n", o->nresults);
  fprintf(fp,"Starting position: %d\n",o->position);
  fprintf(fp,"Enabled Options: ");
  penabled(header);
  penabled(footer);
  penabled(urls);
  penabled(titles);
  penabled(snippets);
  penabled(ads);
  penabled(misc);
  penabled(reverse);
#ifdef HAVE_GNUGOLD
  penabled(prime);
  penabled(broadcast);
  penabled(multicast);
  penabled(ipv4);
  penabled(ipv6);
#endif
  penabled(force);
  penabled(cache);
  penabled(offline);
  penabled(lucky);
  penabled(safe);
  penabled(reg);
  penabled(level);
  penabled(engine);
  penabled(mirror);
  penabled(plugin);
  penabled(dummy);
  penabled(debug);
  fprintf(fp,"\n");
}

#define pifverbose(q,string) if(q->verbose) { printf("%s",val); }

main(int argc, char **argv) {
  int i = 0;
  int cnt = 0;
  char host[1024];
  int querylen = 0;
  QueryOptions_t q;

  gnugol_init_QueryOptions(&q);

  while(FCGI_Accept() >= 0) {
    gnugol_reset_QueryOptions(&q);
    q.nresults = 4;
    q.position = 0;
    q.engine_name = "dummy";
    q.language = "en";
    q.header = 0;
    q.footer = 0;
    q.level = -1;
    q.dummy = 1;
    q.format = FORMATHTML;

    strcpy(q.keywords,"test%ipv6");

    if(!(q.urls | q.snippets | q.ads | q.titles)) {
      q.urls = 1; // Always default to fetching urls
    }

  if(q.dummy) {
    if(engine_dummy(&q) == 0) {
      printf("Content-type: text/html\r\n"
	     "\r\n"
	     "<html><body>%s</body></html>",q.out.s);
    } else {
      fprintf(stderr,"Error %s",q.err.s);
    }

  } else {

  if(engine_bing(&q) == 0) {
      printf("Content-type: text/html\r\n"
	     "\r\n"
	     "<html><body>%s</body></html>",q.out.s);
  } else {
    fprintf(stderr,"%s\n",q.err.s);
  }
  }

  if(q.debug)
    fprintf(stderr,"len = %d\n size = %d, Result = %s\n",q.out.len, q.out.size, q.out.s);
  }
  gnugol_free_QueryOptions(&q);
  return(0);
}

