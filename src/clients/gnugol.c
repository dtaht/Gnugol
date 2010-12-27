/* This code is Copyright (C) 2008 by Michael David Taht 
   and released under the terms of the GNU AFFERO PUBLIC_LICENSE, version 3 
   for details of this license see the COPYRIGHT file */

/* Command line client for gnugol */

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "query.h"
#include "formats.h"
#include "gnugol_engines.h"

#ifndef __GNUC__
#  define __attribute__(x)
#endif

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
  { FORMATTERM, "term" },
  { FORMATTERM, "text" },
  { FORMATSSML, "ssml" },
  { FORMATTEXTILE, "textile" },
  { FORMATRAW,  "raw" },
  { FORMATMDWN, "mdwn" },
  { FORMATMDWN, "md" },
  { FORMATMDWN, "markdown" },
  { 0, NULL },
  };


int usage (char *err) {
  if(err) fprintf(stderr,"%s\n",err);
  printf("gnugol [options] keywords to search for\n");
  printf(
	 "-n --nresults  number of results to fetch\n"
	 "-p --position  start of results to fetch\n"
	 "-o --output    [html|json|org|mdwn|wiki|text|term|ssml|textile|raw]\n"
	 "-e --engine    [bing|google|wikipedia|dummy]\n"
	 "-l --level     X   result formatting level\n"
	 "-H --header    0|1 disable/enable output header\n"
	 "-F --footer    0|1 disable/enable output footer\n"
	 "-d --debug     X   debug output level\n"
	 "-h --help          this message\n"

#ifdef HAVE_GNUGOLD
	 "-P --prime     prime the caches, routes, etc\n"
	 "-R --register\n"
	 "-m --multicast ask for results from local network\n"
	 "-b --broadcast broadcast results to local network\n"
	 "-6 --ipv6 listen on ipv6\n"
	 "-4 --ipv4 listen on ipv4\n"
	 "-F --dontfork don't fork off the server\n"
	 "-S --Secure    use secure transport\n"
	 "-T --trust networks\n"
#endif

#ifdef WHENIHAVETIMETOADDTHESEOFFICIALLY
	 "-a --ads 0|1 "
	 "-t --titles 0|1\n"
	 "-u --urls 0|1 "
	 "-s --snippets 0|1\n"
	 "-r --reverse   reverse the list. \n"
	 "-i --input     [filename] input from a file\n"
	 "-c --cache     serve only results from cache(s)\n"
	 "-O --Offline   store up query for later\n"
	 "-f --force     force a new query, even if cached\n"
	 "--defaults     show the defaults\n"
	 "--source       fetch the source code this was compiled with\n"
	 "--config "
	 "--copyright "
	 "--license\n"
#endif
	 "--verbose          provide more verbose results\n");
  exit(-1);
}

// There's a version of getopt out there that lets you put in the help text.
// Silly to separate the two, but...

static struct option long_options[] = {
  {"reverse", 0, 0, 'r' }, 
  {"urls", 1, 0, 'u' },
  {"snippets", 1, 0,'s'},
  {"ads", 1,0,'a' },
  {"titles", 1,0, 't'},
  {"level", 1, 0, 'l' },
  {"engine", 1,0, 'e'},
  {"register", 2,0, 'R'},
  {"input", 1,0, 'i'},
  {"plugin", 1,0, 'g'},    
  {"lucky", 0,0, 'L'},     
#ifdef HAVE_GNUGOLD
  {"prime", 0,0, 'P'},     
  {"multicast", 2,0, 'm'}, 
  {"broadcast", 0,0, 'b'}, 
  {"cache", 0,0, 'c'},     
  {"secure", 0,0, 'Z'},  
  {"trust", 0,0, 'T'},  
  {"ipv6", 0,0, '6'},   
  {"ipv4", 0,0, '4'},   
#endif
  {"footer", 1,0, 'F'},   
  {"header", 1,0, 'H'},   
  {"output", 1,0, 'o'},  
  {"Offline", 0,0, '5'}, 
  {"force", 0,0, 'f'},   
  {"nresults", 1,0, 'n'},
  {"position", 1,0, 'p'},
  {"verbose", 0,0, 'v'},   
  {"debug", 1,0, 'd'},   
  {"defaults", 0,0, 'D'},   
  {"source", 0,0, 0},     
  {"safe", 0,0, 'S'},       
  {"help", 0,0, 'h'},       
  {"config", 0,0,'C'},
  {0,0,0,0},
};

int gnugol_parse_config_file(QueryOptions_t *q __attribute__((unused))) {
  return 0;
} 

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
  penabled(debug);
  fprintf(fp,"\n");
  return 0;
}

#define pifverbose(q,string) if(q->verbose) { printf("%s",val); }

int process_options(int argc, char **argv, QueryOptions_t *o) {
  int option_index = 0;
  int i = 0;
  int querylen = 0;
  int opt = 0;
  if(argc == 1) usage("");

#ifdef HAVE_GNUGOLD
  // FIXME, not all opt defined, some extras
#define QSTRING "7654C:ru:s:a:t:e:Ri:PlmS:bco:fOZTDd:vU:jn:p:SH:F:"
#else
#define QSTRING "7654C:ru:s:a:t:e:Ri:PlmS:bco:fOZTDd:vU:jn:p:SH:F:"
#endif  
// useful a -- by itself ends options parsing

  do {
    opt = getopt_long(argc, argv, 
		      QSTRING,
		      long_options, &option_index);
    if(opt == -1) break; 
    switch (opt) { 
    case 'r': o->reverse = 1; break;  
    case 'u': o->urls = strtoul(optarg,NULL,10); break;
    case 's': o->snippets = strtoul(optarg,NULL,10); break;
    case 'a': o->ads = strtoul(optarg,NULL,10); break;
    case 't': o->titles = strtoul(optarg,NULL,10); break;
    case 'H': o->header = strtoul(optarg,NULL,10); break;
    case 'F': o->footer = strtoul(optarg,NULL,10); break;
    case 'p': o->position = strtoul(optarg,NULL,10); break;
    case 'T': o->trust = 1; break;
    case 'e': o->engine = 1; o->engine_name = optarg; break; 
    case 'R': o->reg = 1; break;
    case 'i': o->input = 1; o->input_file = optarg; break; // FIXME
    case 'P': o->plugin = 1; break;
    case 'L': o->lucky = 1; break;
    case 'm': o->multicast = 1; break;
    case 'b': o->broadcast = 1; break;
    case 'c': o->cache = 1; break;
    case 'o': {
      int i;
      for(i = 0; output_type[i].desc != NULL; i++)
	if(strcmp(output_type[i].desc,optarg) == 0) o->format = output_type[i].id; 
    }
      break;
    case '5': o->offline = 1; break;
    case 'f': o->output = 1; break; // FIXME
    case 'l': o->level = strtoul(optarg,NULL,10); break;
    case 'n': o->nresults = strtoul(optarg,NULL,10); break; 
    case 'Z': o->secure = 1; break; // unimplemented
    case 'S': o->safe = 1; strtoul(optarg,NULL,10); break; 
    case 'd': o->debug = strtoul(optarg,NULL,10); break;
    case 'v': o->verbose = 1; break;
    case '6': o->ipv6 = 1; break;
    case '4': o->ipv4 = 1; break;
    case 'h': 
    case '?': usage(NULL); break;

    default: fprintf(stderr,"%c",opt); usage("Invalid option"); break;
    } 
  } while (1); 

  for(i = optind; i < argc; i++) {
    if((querylen += strlen(argv[i]) > MAX_MTU - 80)) {
      fprintf(stderr,"Too many words in query, try something smaller\n");
      return(1);
    }
    strcat(o->keywords,argv[i]);
    if(i+1 < argc) strcat(o->keywords,"%20"); // FIXME find urlencode lib
  }
  // FIXME: if called with no args do the right thing
  if(o->debug > 0) print_enabled_options(o, stderr);
  if(!(o->urls | o->snippets | o->ads | o->titles)) { 
    o->urls = 1; // Always default to fetching urls 
  }
  return(optind);
}

static void gnugol_default_QueryOptions(QueryOptions_t *q) {
  q->nresults = 4;
  q->position = 0;
  q->urls = 1;
  q->snippets = 1;
  q->titles = 1;
  q->engine_name = "google";
  q->language = "en";
  q->header = 1;
  q->footer = 1;
  q->format = FORMATDEFAULT; // NONE
  q->level = -1;
}

int main(int argc, char **argv) {
  QueryOptions_t q;
  gnugol_init_QueryOptions(&q);
  gnugol_default_QueryOptions(&q);
  process_options(argc,argv,&q);

  int result = gnugol_query_engine(&q);

  if(q.returned_results > 0) {     
      printf("%s",q.out.s);
    }

  if(result < 0 || q.debug) {
    fprintf(stderr,"Errors: %s\nWarnings:%s\n",q.err.s,q.wrn.s);
  }

  if(q.debug > 10) {
    fprintf(stderr,"out len = %d\n size = %d, Contents = %s\n",q.out.len, q.out.size, q.out.s);
    fprintf(stderr,"wrn len = %d\n size = %d, Contents = %s\n",q.wrn.len, q.wrn.size, q.wrn.s);
    fprintf(stderr,"err len = %d\n size = %d, Contents  = %s\n",q.err.len, q.err.size, q.err.s);
  }
  gnugol_free_QueryOptions(&q);
  return(0); 
}
