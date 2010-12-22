/* This code is Copyright (C) 2008 by Michael David Taht 
   and released under the terms of the GNU AFFERO PUBLIC_LICENSE, version 3 
   for details of this license see the COPYRIGHT file */

/* Command line client for gnugol */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
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
  { FORMATTEXTILE, "textile" },
  { FORMATRAW,  "raw" },
  { FORMATMDWN, "mdwn" },
  { FORMATMDWN, "md" },
  { FORMATMDWN, "markdown" },
  { 0, NULL },
  };


/*
FIXME: Make this a dynamic engine callback at runtime

static struct engine_callbacks engine[] = {
  { "dummy", engine_dummy },
  { "google", engine_googlev1 },
  { NULL },
};

*/

int usage (char *err) {
  if(err) fprintf(stderr,"%s\n",err);
  printf("gnugol [options] keywords\n");
  printf("-r --reverse   reverse the list. \n"
	 "-u --urls "
	 "-s --snippets "
	 "-a --ads "
	 "-t --titles\n"
	 "-e --engine    use an alternate engine\n"
	 "-i --input [filename] input from a file\n"
	 "-p --plugin    use an alternate plugin\n"
	 "-L --lucky     autofetch the first result\n"
#ifdef HAVE_GNUGOLD
	 "-P --prime     prime the caches, routes, etc\n"
	 "-R --register\n"
	 "-m --multicast ask for results from local network\n"
	 "-b --broadcast broadcast results to local network\n"
	 "-6 --ipv6 listen on ipv6\n"
	 "-4 --ipv4 listen on ipv4\n"
	 "-F --dontfork don't fork off the server\n"
	 "-S --Secure    use secure transport\n"
	 "-B --dummy dummy input (useful for stress testing)\n"
	 "-T --trust networks\n"
#endif
	 "-l --level X   result level\n"
	 "-c --cache     serve only results from cache(s)\n"
	 "-O --Offline   store up query for later\n"
	 "-f --force     force a new query, even if cached\n"
	 "-n --nresults  number of results to fetch\n"
	 "-p --position  start of results to fetch\n"
	 "-o --output [html|json|xml|org|mdwn|wiki|wiki|ssml|textile]\n"
	 "-d --debug  [level]    Debug output\n"
	 "--defaults     show the defaults\n"
	 "--source       fetch the source code this was compiled with\n"
	 "--help         this message\n"
	 "--config "
	 "--verbose "
	 "--copyright "
	 "--license\n");
  exit(-1);
}

// There's a version of getopt out there that lets you put in the help text.
// Silly to separate the two, but...

static struct option long_options[] = {
  {"reverse", 0, 0, 'r' }, 
  {"urls", 0, 0, 'u' },
  {"snippets", 0, 0,'s'},
  {"ads", 0,0,'a' },
  {"level", 1, 0, 'l' },
  {"titles", 0,0, 't'},
  {"engine", 1,0, 'e'},
  {"register", 2,0, 'r'},
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
  {"dontfork", 0,0, 'F'},   
#endif
  {"output", 1,0, 'o'},  
  {"Offline", 0,0, '5'}, 
  {"force", 0,0, 'f'},   
  {"nresults", 1,0, 'n'},
  {"position", 1,0, 'U'},
  {"verbose", 0,0, 'v'},   
  {"dummy", 0,0, '9'},   
  {"debug", 1,0, 'd'},   
  {"defaults", 0,0, 'D'},   
  {"source", 0,0, 0},     
  {"safe", 0,0, 'S'},       
  {"help", 0,0, 'h'},       
  {"config", 0,0,'C'},
  {"dummy", 0,0,'B'},
  {0,0,0,0},
};

parse_config_file(QueryOptions_t *q) {

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
  penabled(dummy);
  penabled(debug);
  fprintf(fp,"\n");
}

#define pifverbose(q,string) if(q->verbose) { printf("%s",val); }

int process_options(int argc, char **argv, QueryOptions_t *o) {
  int option_index = 0;
  int i = 0;
  int querylen = 0;
  int opt = 0;
  int count = 0;
  if(argc == 1) usage("");

#ifdef HAVE_GNUGOLD
  // FIXME, not all opt defined, some extras
#define QSTRING "97654C:rusate:Ri:PplmS:bco:fdOZFTDd:vU:jn:p:S"
#else
#define QSTRING "97654C:rusate:Ri:PplmS:bco:fdOZFTDd:vU:jn:p:S"
#endif  
  do {
    opt = getopt_long(argc, argv, 
		      QSTRING,
		      long_options, &option_index);
    if(opt == -1) break;

    switch (opt) { 
    case 'r': o->reverse = 1; break;  
    case 'u': o->urls = 1; break;
    case 's': o->snippets = 1; break;
    case 'a': o->ads = 1; break;
    case 't': o->titles = 1; break;
    case 'T': o->trust = 1; break;
    case 'e': o->engine = 1; o->engine_name = optarg; break; 
    case 'R': o->reg = 1; break;
    case 'i': o->input = 1; o->input_file = optarg; break; // FIXME
    case 'P': o->prime = 1; break;
    case 'p': o->plugin = 1; break;
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
    case 'f': o->output = 1; break;
    case 'l': o->level = atoi(optarg); break;
    case 'n': o->nresults = atoi(optarg); break; 
    case 'U': o->position = atoi(optarg); break; 
    case 'Z': o->secure = 1; break; // unimplemented
    case 'S': o->safe = 1; atoi(optarg); break; 
    case 'd': o->debug = atoi(optarg); break;
    case 'F': o->dontfork = 1; break;
    case 'v': o->verbose = 1; break;
    case '9': o->dummy = 1; break;
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
    if(i+1 < argc) strcat(o->keywords,"%20");
  }
  // FIXME: if called with no args do the right thing
  if(o->debug > 0) print_enabled_options(o, stderr);
  return(optind);
}

main(int argc, char **argv) {
  int i = 0;
  int cnt = 0;
  char host[1024];
  int querylen = 0;
  QueryOptions_t q;
  gnugol_init_QueryOptions(&q);
  // Defaults

  q.nresults = 4;
  q.position = 0;
  q.engine_name = "googlev1";
  q.language = "en";
  q.header = 1;
  q.footer = 1;
  q.format = FORMATDEFAULT; // NONE
  q.level = -1;

  process_options(argc,argv,&q);
  
  if(!(q.urls | q.snippets | q.ads | q.titles)) { 
    q.urls = 1; // Always default to fetching urls 
  }

  if(q.dummy) {
    if(engine_dummy(&q) == 0) {
      printf("%s",q.out.s);
    } else {
      fprintf(stderr,"Error %s",q.err.s);
    }
    
  } else {
  
  if(engine_googlev1(&q) == 0) {
    printf("%s",q.out.s);
  } else {
    fprintf(stderr,"%s\n",q.err.s);
  }
  }

  if(q.debug)
    printf("len = %d\n size = %d, Result = %s\n",q.out.len, q.out.size, q.out.s);
  gnugol_free_QueryOptions(&q);
  return(0); 
}
