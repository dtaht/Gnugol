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
  const enum gnugol_formatter id;
  const char *const desc;
};

// FIXME: Verify differences between ikiwiki and media wiki format

static const struct output_types output_type[] = {
  { FORMATHTML5, "html5" },
  { FORMATHTML, "html" },
  { FORMATIKI, "iki" },
  { FORMATWIKI, "wiki" },
  { FORMATXML,  "xml" },
  { FORMATJSON, "json" },
  { FORMATORG,  "org" },
  { FORMATTERM, "term" },
  { FORMATTEXT, "text" },
  { FORMATSSML, "ssml" },
  { FORMATTEXTILE, "textile" },
  { FORMATRAW,  "raw" },
  { FORMATMDWN, "mdwn" },
  { FORMATMDWN, "md" },
  { FORMATMDWN, "markdown" },
  { FORMATINFO, "info" },
  { FORMATCSV, "csv" },
  { FORMATRAW, "raw" },
  { FORMATSQL, "sql" },
  { FORMATMAN, "man" },
  { FORMATDNS, "dns" },
  { 0, NULL },
  };

int usage (char *err) {
  if(err) fprintf(stderr,"%s\n",err);
  printf("gnugol [options] keywords to search for\n");
  printf(
	 "-a --about         [credits|copyright|license|source|config|manual|stats]\n"
	 "-d --debug     X   debug output level\n"
	 "-e --engine        [bing|google|dummy]\n"
	 "-F --footer    0|1 disable/enable output footer\n"
	 "-H --header    0|1 disable/enable output header\n"
	 "-h --help          this message\n"
	 "-i --indent    X   result indentation level\n"
	 "-l --language-in   [language (in UTF-8 format: e.g. en_US)]\n"
	 "-L --language-out  [language (in UTF-8 format: e.g. en_US)]\n"
	 "-n --nresults      number of results to fetch\n"
	 "-o --output        [html|json|org|mdwn|wiki|text|term|ssml|textile|raw]\n"
	 "-p --position      start of results to fetch\n"
	 "-s --snippets  0|1 disable/enable snippets\n"
	 "-t --titles    0|1 disable/enable titles\n"
	 "-u --urls      0|1 disable/enable urls\n"
	 "-v --verbose       provide more verbose insight\n"

#ifdef HAVE_GNUGOLD
	 "-b --broadcast broadcast results to local network\n"
	 "   --dontfork don't fork off the server\n"
	 "-4 --ipv4 listen on ipv4\n"
	 "-6 --ipv6 listen on ipv6\n"
	 "-m --multicast ask for results from local network\n"
	 "-P --prime     prime the caches, routes, etc\n"
	 "-R --register\n"
	 "-S --Secure    use secure transport\n"
	 "-T --trust networks\n"
#endif

#ifdef WHENIHAVETIMETOADDTHESEOFFICIALLY
	 "-A --ads 0|1\n"
	 "-c --cache     serve only results from cache(s)\n"
	 "-f --force     force a new query, even if cached\n"
	 "-I --input     [filename] input from a file\n"
	 "-5 --offline   store up query for later\n"
	 "-r --reverse   reverse the list\n"
#endif

#ifdef WHATABOUTTHESE
	 "-C --config\n"
	 "-D --defaults\n"
	 "-g --plugin\n"
	 "-S --safe\n"
	 "   --source\n"
#endif
  );

  exit(-1);
}

static const struct option long_options[] = {
  { "about"		, no_argument		, NULL , 'a' } ,
  { "debug"		, required_argument	, NULL , 'd' } ,
  { "engine"		, required_argument	, NULL , 'e' } ,
  { "footer"		, required_argument	, NULL , 'F' } ,
  { "header"		, required_argument	, NULL , 'H' } ,
  { "help"		, no_argument		, NULL , 'h' } ,
  { "indent"		, required_argument	, NULL , 'i' } ,
  { "language-in"	, required_argument	, NULL , 'l' } ,
  { "language-out"	, required_argument	, NULL , 'L' } ,
  { "nresults"		, required_argument	, NULL , 'n' } ,
  { "output"		, required_argument	, NULL , 'o' } ,
  { "position"		, required_argument	, NULL , 'p' } ,
  { "snippets"		, required_argument	, NULL , 's' } ,
  { "titles"		, required_argument	, NULL , 't' } ,
  { "urls"		, required_argument	, NULL , 'u' } ,
  { "verbose"		, no_argument		, NULL , 'v' } ,

#ifdef HAVE_GNUGOLD
  { "broadcast"		, no_argument		, NULL , 'b' } ,
  { "dontfork"		, no_argument		, NULL ,  0  } ,
  { "ipv4"		, no_argument		, NULL , '4' } ,
  { "ipv6"		, no_argument		, NULL , '6' } ,
  { "multicast"		, no_argument		, NULL , 'm' } ,
  { "prime"		, no_argument		, NULL , 'P' } ,
  { "register"		, no_argument		, NULL , 'R' } ,
  { "secure"		, no_argument		, NULL , 'S' } ,
  { "trust"		, no_argument		, NULL , 'T' } ,
#endif

#ifdef WHENIHAVETIMETOADDTHESEOFFICIALLY
  { "ads"		, required_argument	, NULL , 'A' } ,
  { "cache"		, no_argument		, NULL , 'c' } ,     
  { "force"		, no_argument		, NULL , 'f' } ,   
  { "input"		, required_argument	, NULL , 'I' } ,
  { "offline"		, no_argument		, NULL , '5' } , 
  { "reverse"		, no_argument		, NULL , 'r' } , 
#endif

#ifdef WHATABOUTTHESE
  { "config"		, no_argument		, NULL , 'C' } ,
  { "defaults"		, no_argument		, NULL , 'D' } ,
  { "plugin"		, no_argument		, NULL , 'g' } ,
  { "safe"		, no_argument		, NULL , 'S' } ,
  { "source"		, no_argument		, NULL ,  0  } ,
#endif

  { NULL		, 0			, NULL ,  0  }
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
  penabled(safe);
  penabled(reg);
  penabled(indent);
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
#  define QSTRING "ad:e:F:H:hi:l:L:n:o:p:s:t:u:vb46mPRST"
#else
#  define QSTRING "ad:e:F:H:hi:l:L:n:o:p:s:t:u:v"
#endif

// useful a -- by itself ends options parsing

  do {
    opt = getopt_long(argc, argv, 
		      QSTRING,
		      long_options, &option_index);
    if(opt == -1) break; 

    switch (opt) 
    { 
      case 'a': o->about = 1;  break; 
      case 'd': o->debug = strtoul(optarg,NULL,10); break;
      case 'e': o->engine = 1; o->engine_name = optarg; break; 
      case 'F': o->footer = strtoul(optarg,NULL,10); break;
      case 'H': o->header = strtoul(optarg,NULL,10); break;
      case 'h': 
      case '?': usage(NULL); break;
      case 'i': o->indent = strtoul(optarg,NULL,10); break;
      case 'l': o->input_language = optarg; break;
      case 'L': o->output_language = optarg; break;
      case 'n': o->nresults = strtoul(optarg,NULL,10); break; 
      case 'o':
           for(int i = 0; output_type[i].desc != NULL; i++)
	    if(strcmp(output_type[i].desc,optarg) == 0) 
	      o->format = output_type[i].id; 
           break;
      case 'p': o->position = strtoul(optarg,NULL,10); break;
      case 's': o->snippets = strtoul(optarg,NULL,10); break;
      case 't': o->titles = strtoul(optarg,NULL,10); break;
      case 'u': o->urls = strtoul(optarg,NULL,10); break;
      case 'v': o->verbose = 1; break;

#ifdef HAVE_GNUGOLD
      case 'b': o->broadcast = 1; break;
      /* dontfork missing */
      case '4': o->ipv4 = 1; break;
      case '6': o->ipv6 = 1; break;
      case 'm': o->multicast = 1; break;
      /* prime missing */
      case 'R': o->reg = 1; break;
      case 'Z': o->secure = 1; break; // unimplemented
      case 'T': o->trust = 1; break;
#endif

#ifdef WHENIHAVETIMETOADDTHESEOFFICIALLY
      case 'A': o->ads = strtoul(optarg,NULL,10); break;
      case 'c': o->cache = 1; break;
      /* force missing */
      case 'I': o->input = 1; o->input_file = optarg; break; // FIXME
      case '5': o->offline = 1; break;
      case 'r': o->reverse = 1; break;  
#endif

#ifdef WHATABOUTTHESE
      /* config missing */
      /* defaults missing */
      case 'P': o->plugin = 1; break;
      case 'f': o->output = 1; break; // FIXME
      case 'S': o->safe = 1; strtoul(optarg,NULL,10); break; 
      /* source missing */
#endif

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
  q->nresults = 5;
  q->position = 0;
  q->urls = 1;
  q->snippets = 1;
  q->titles = 1;
  q->engine_name = "google";
  q->input_language = "en";
  q->header = 1;
  q->footer = 1;
  q->format = FORMATDEFAULT; // NONE
  q->indent = -1;
}

int main(int argc, char **argv) {
  int result;
  QueryOptions_t q;
  gnugol_init_QueryOptions(&q);
  gnugol_default_QueryOptions(&q);
  process_options(argc,argv,&q);
  if(q.about) {
    q.engine_name = "credits";  
    q.header_str = "About";
  } 

  result = gnugol_query_engine(&q);

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
