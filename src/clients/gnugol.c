/* Command line client for gnugol *
/* This code is Copyright (C) 2008 by Michael David Taht 
   and released under the terms of the GNU AFFERO PUBLIC_LICENSE, version 3 
   for details of this license see the COPYRIGHT file */

#include <stdio.h>
#include <unistd.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "query.h"

/* FIXME - look over wget options for useful stuff

*/

int usage () {
 printf("gnugol [options] keywords\n");
 printf("-r --reverse   unreverse the list. The default IS reversed already\n");
 printf("-u --urls\n");
 printf("-s --snippets\n");
 printf("-a --ads\n");
 printf("-t --titles\n");
 printf("-e --engine    use an alternate engine\n");
 printf("-R --register\n");
 printf("-i --input     input from a file\n");
 printf("-P --prime     prime the caches, routes, etc\n");
 printf("-p --plugin    use an alternate plugin\n");
 printf("-l --lucky     autofetch the first result\n");
 printf("-m --multicast ask for results from local network\n");
 printf("-b --broadcast broadcast results to local network\n");
 printf("-c --cache     serve only results from cache(s)\n");
 printf("-o --output    output to a file\n");
 printf("-O --Offline   store up query for later\n");
 printf("-f --force     force a new query, even if cached\n");
 printf("-n --nresults  number of results to fetch\n");
 printf("-p --position  start of results to fetch\n");
 printf("-S --Secure    use secure transport\n");
 printf("-H --html      output html\n");
 printf("-X --xml       output gnugol XML\n");
 printf("--defaults     show the defaults\n");
 printf("--source       fetch the source code this was compiled with\n");
 printf("--help         this message\n");
 printf("--config"); 
 printf(" --verbose"); 
 printf(" --copyright"); 
 printf(" --license\n"); 
 exit(-1);
}

// There's a version of getopt out there that lets you put in the help text.
// Silly to seperate the two.

static struct option long_options[] = {
  {"reverse", 0, 0, 'r' }, 
  {"urls", 0, 0, 'u' },
  {"snippets", 0, 0,'s'},
  {"ads", 0,0,'a' },
  {"titles", 0,0, 't'},
  {"engine", 1,0, 'e'},
  {"register", 2,0, 'r'},
  {"input", 1,0, 'i'},
  {"prime", 0,0, 'P'},     
  {"plugin", 1,0, 'g'},    
  {"lucky", 0,0, 'l'},     
  {"multicast", 2,0, 'm'}, 
  {"broadcast", 0,0, 'b'}, 
  {"cache", 0,0, 'c'},     
  {"output", 1,0, 'o'},  
  {"Offline", 0,0, 'O'}, 
  {"force", 0,0, 'f'},   
  {"nresults", 1,0, 'n'},
  {"position", 1,0, 'p'},
  {"secure", 0,0, 'S'},  
  {"html", 0,0, 'H'},    
  {"xml", 0,0, 'X'},     
  {"verbose", 0,0, 'v'},   
  {"debug", 0,0, 'd'},   
  {"defaults", 0,0, 'D'},   
  {"source", 0,0, 0},     
  {"help", 0,0, 'h'},       
  {"config", 0,0,'C'},
};

parse_config_file(QueryOptions *q) {
} 

//Sometimes I wish the c preprocessor had exited the 80s
// #define penabled(a,b) fprintf(stderr,a "%s", o->b ? "enabled":"")
#define penabled(a,b) if(o->b) fprintf(stderr,a " ");

int 
print_enabled_options(QueryOptions *o) {
  if(o->verbose) {
    fprintf(stderr,"Options: ");
    penabled("urls",urls);
    penabled("titles",titles);
    penabled("snippets",snippets);
    penabled("ads",ads);
    penabled("misc",misc);
    penabled("reverse",reverse);
    penabled("broadcast",broadcast);
    penabled("multicast",multicast);
    penabled("force",force);
    penabled("cache",cache);
    penabled("xml",xml);
    penabled("html",html);
    penabled("offline",offline);
    penabled("lucky",lucky);
    penabled("register",reg);
    penabled("prime",prime);
    penabled("engine",engine);
    penabled("mirroring",mirror);
    penabled("plugin",plugin);
    fprintf(stderr,"\n");
    fprintf(stderr,"Result Request: %d\n", o->nresults);
    fprintf(stderr,"Position: %d\n",o->position);
  }
}

#define pifverbose(q,string) if(q->verbose) { printf("%s",val); }

int process_options(int argc, char **argv, QueryData *q) {
  QueryOptions *o = &q->options; 
  int option_index;
  int opt = 0;
  int count = 0;
  printf("%d %s\n",argc,argv[2]);
  // FIXME: Parse optional arguments
  while (1) {
    opt = getopt_long(argc, argv, 
		      "rusateRiPplmbcoOfn:pSHX",
		      long_options, &option_index);
    if(opt == -1) break;
    //    fprintf(stderr, "option_index: %d\n opt: %c\n", option_index, opt);
    switch (opt) { 
      //   case 0: fprintf(stderr,"option %s", long_options[option_index].name);
      // if(optarg) fprintf(stderr, " with arg %s", optarg);
      // fprintf(stderr,"\n");
      // break;
    case 'r': o->reverse = 1; break;  
    case 'u': o->urls = 1; break;
    case 's': o->snippets = 1; break;
    case 'a': o->ads = 1; break;
    case 't': o->titles = 1; break;
    case 'e': o->engine =1; break; // FIXME strcpy engine type
    case 'R': o->reg = 1; break;
    case 'i': o->input = 1; break; // FIXME
    case 'P': o->prime = 1; break;
    case 'p': o->plugin = 1; break;
    case 'l': o->lucky = 1; break;
    case 'm': o->multicast = 1; break;
    case 'b': o->broadcast = 1; break;
    case 'c': o->cache = 1; break;
    case 'o': o->output = 1; break;
    case 'O': o->offline = 1; break;
    case 'f': o->force = 1; break;
    case 'n': o->nresults = 1; break; // FIXME
    case 'U': o->position = 1; break; // Another obvious fixme 
    case 'S': o->secure = 1; break; // unimplemented
    case 'H': o->html = 1; break; 
    case 'X': o->xml = 1; break;
    case 'h': usage(); break;
    case 'v': o->verbose = 1; break;
    default: break;
    }
  }
      /* '--trust'       trust networks
	 '--defaults     show the defaults\n");
	 '--source       fetch the source code this was compiled with\n");
	 '--help         this message\n");
	 '--config"); printf(" --verbose"); printf(" --copyright"); printf(" --license\n"); 
*/
  return(optind);
}

main(int argc, char **argv) {
	int i = 0;
	if(argc == 1) usage();
	int querylen = 0;
  	QueryData *q = (QueryData *) calloc(sizeof(QueryData),1);

	// Defaults
	q->options.nresults = 10;
	q->options.position = 0;
	q->options.engine_name = "google";
	q->options.language = "en";
	q->options.urls = 1; // Always default to fetching urls

	process_options(argc,argv,q);
	
	for(i = optind; i < argc; i++) {
	  if((querylen += strlen(argv[i]) > MAX_MTU - 80)) {
	    fprintf(stderr,"Too many words in query, try something smaller\n");
	    free(q);
	    exit(-1);
	  }
	  strcat(q->keywords,argv[i]);
	  strcat(q->keywords," ");
	}

	print_enabled_options(&q->options);
	if(q->options.verbose) fprintf(stderr,"Keywords: %s\n",q->keywords);
	
#ifdef DUMMY_CLIENT
	strcpy(q->keywords,"WHAT THE HECK?");
	int cnt = query_main(q,"localhost");
#else
	char host[1024];
	int cnt;
	char *h = getenv("GNUGOL_SERVER");
	if ( h != NULL) {
	  strcpy(host,h);
	} else {
	  strcpy(host,"localhost");
	}
	cnt = query_main(q,host);
#endif
	for (i = 0; i <= cnt-1; i++) {
	  printf("<a href=%s>%s</a><br>", q->links[i], q->snippets[i]); 
	}
	printf("\n");
 }

