/* This code is Copyright (C) 2008 by Michael David Taht 
   and released under the terms of the GNU AFFERO PUBLIC_LICENSE, version 3 
   for details of this license see the COPYRIGHT file */

/* Command line client for gnugol */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "formats.h"
#include "query.h"

extern int plugin_googlev1(QueryOptions *q);

int usage (char *err) {
  if(err) fprintf(stderr,"%s\n",err);
  printf("gnugol [options] keywords\n");
  printf("-r --reverse   unreverse the list. The default IS reversed already\n"
	 "-u --urls "
	 "-s --snippets "
	 "-a --ads "
	 "-t --titles\n"
	 "-e --engine    use an alternate engine\n"
	 "-R --register\n"
	 "-i --input [filename] input from a file\n"
	 "-P --prime     prime the caches, routes, etc\n"
	 "-p --plugin    use an alternate plugin\n"
	 "-l --lucky     autofetch the first result\n"
	 "-m --multicast ask for results from local network\n"
	 "-b --broadcast broadcast results to local network\n"
	 "-c --cache     serve only results from cache(s)\n"
	 "-o --output    output to a file\n"
	 "-O --Offline   store up query for later\n"
	 "-f --force     force a new query, even if cached\n"
	 "-n --nresults  number of results to fetch\n"
	 "-p --position  start of results to fetch\n"
	 "-S --Secure    use secure transport\n"
	 "\nOutput Options:\n"
	 "-H --html      output html\n"
	 "-J --json      output json\n"
	 "-X --xml       output gnugol XML\n"
	 "-O --org       output org format\n"
	 "-M --mdwn      output markdown format\n"
	 "-W --wiki      output ikiwiki format\n"
	 "-X --xml       output gnugol XML format\n"
	 "-B --ssml      output SSML format\n"
	 "-d --debug [level]    Debug output\n"
	 "-B --dummy dummy input (useful for stress testing)\n"
	 "-6 --ipv6 listen on ipv6\n"
	 "-4 --ipv4 listen on ipv4\n"
	 "-F --dontfork don't fork off the server\n"
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
  {"links", 0, 0, 'l' },
  {"snippets", 0, 0,'s'},
  {"ads", 0,0,'a' },
  {"titles", 0,0, 't'},
  {"engine", 1,0, 'e'},
  {"register", 2,0, 'r'},
  {"input", 1,0, 'i'},
  {"prime", 0,0, 'P'},     
  {"plugin", 1,0, 'g'},    
  {"lucky", 0,0, 'L'},     
  {"multicast", 2,0, 'm'}, 
  {"broadcast", 0,0, 'b'}, 
  {"cache", 0,0, 'c'},     
  {"output", 1,0, 'o'},  
  {"Offline", 0,0, '5'}, 
  {"force", 0,0, 'f'},   
  {"nresults", 1,0, 'n'},
  {"position", 1,0, 'U'},
  {"secure", 0,0, 'Z'},  
  {"trust", 0,0, 'T'},  

  {"html", 0,0, 'H'},    
  {"xml", 0,0, 'X'},     
  {"org", 0,0, 'O'},     
  {"wiki", 0,0, 'W'},     
  {"json", 0,0, 'J'},     
  {"text", 0,0, '7'},     
  {"ssml", 0,0, 'B'},     
  {"mdwn", 0,0, 'M'},     

  {"verbose", 0,0, 'v'},   
  {"debug", 1,0, 'd'},   
  {"defaults", 0,0, 'D'},   
  {"ipv6", 0,0, '6'},   
  {"ipv4", 0,0, '4'},   
  {"dontfork", 0,0, 'F'},   
  {"source", 0,0, 0},     
  {"safe", 0,0, 'S'},       
  {"help", 0,0, 'h'},       
  {"config", 0,0,'C'},
  {"dummy", 0,0,'B'},
};

parse_config_file(QueryOptions *q) {

} 

#define penabled(a) if(o->a) fprintf(fp," " # a " ");

int 
print_enabled_options(QueryOptions *o, FILE *fp) {
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
  penabled(broadcast);
  penabled(multicast);
  penabled(force);
  penabled(cache);
  penabled(json);
  penabled(org);
  penabled(wiki);
  penabled(mdwn);
  penabled(xml);
  penabled(html);
  penabled(ssml);
  penabled(offline);
  penabled(lucky);
  penabled(safe);
  penabled(reg);
  penabled(prime);
  penabled(engine);
  penabled(mirror);
  penabled(plugin);
  penabled(ipv4);
  penabled(ipv6);
  penabled(dummy);
  penabled(debug);
  fprintf(fp,"\n");
}

#define pifverbose(q,string) if(q->verbose) { printf("%s",val); }

int process_options(int argc, char **argv, QueryData *q) {
  QueryOptions *o = &q->options; 
  int option_index = 0;
  int i = 0;
  int querylen = 0;
  int opt = 0;
  int count = 0;
  if(argc == 1) usage("");
  
  do {
    opt = getopt_long(argc, argv, 
		      "7654C:rusate:Ri:PplmS:bcoOfdOZFTBWDd:vU:jn:p:SHX",
		      long_options, &option_index);
    if(opt == -1) break;

    switch (opt) { 
    case 'r': o->reverse = 1; break;  
    case 'u': o->urls = 1; break;
    case 's': o->snippets = 1; break;
    case 'a': o->ads = 1; break;
    case 't': o->titles = 1; break;
    case 'T': o->trust = 1; break;
    case 'e': o->engine = 1; o->engine_name = optarg; break; // FIXME strcpy engine type
    case 'R': o->reg = 1; break;
    case 'i': o->input = 1; o->input_file = optarg; break; // FIXME
    case 'P': o->prime = 1; break;
    case 'p': o->plugin = 1; break;
    case 'L': o->lucky = 1; break;
    case 'm': o->multicast = 1; break;
    case 'b': o->broadcast = 1; break;
    case 'c': o->cache = 1; break;
    case 'o': o->output = 1; break;
    case '5': o->offline = 1; break;
    case 'f': o->force = 1; break;
    case 'n': o->nresults = atoi(optarg); break; 
    case 'U': o->position = atoi(optarg); break; 
    case 'Z': o->secure = 1; break; // unimplemented
    case 'S': o->safe = 1; atoi(optarg); break; 
    case 'J': o->json = 1; break; 
    case 'H': o->html = 1; break; 
    case 'X': o->xml = 1; break;
    case 'O': o->org = 1; break;
    case 'W': o->wiki = 1; break;
    case '7': o->text = 1; break;
    case '8': o->text = 1; break;
    case 'B': o->ssml = 1; break;
    case 'M': o->mdwn = 1; break;
    case 'd': o->debug = atoi(optarg); break;
    case 'F': o->dontfork = 1; break;
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
      free(q);
      exit(-1);
    }
    strcat(o->keywords,argv[i]);
    if(i+1 < argc) strcat(o->keywords,"%20");
  }

  if(q->options.debug > 0) print_enabled_options(&q->options, stderr);
  if(q->options.html + q->options.xml + q->options.json + q->options.mdwn +
     q->options.org + q->options.text + q->options.wiki + q->options.ssml > 1) { 
    usage("You can only select one of json, xml, org, mdwn, text, wiki, html, or ssml");
  } 

  if(q->options.html) q->options.format = FORMATHTML;
  if(q->options.xml)  q->options.format = FORMATXML;
  if(q->options.json) q->options.format = FORMATJSON; 
  if(q->options.org)  q->options.format = FORMATORG;
  if(q->options.text) q->options.format = FORMATTERM; 
  if(q->options.wiki) q->options.format = FORMATWIKI;
  if(q->options.ssml) q->options.format = FORMATSSML;
  if(q->options.mdwn) q->options.format = FORMATMDWN;

  return(optind);
}

main(int argc, char **argv) {
  int i = 0;
  int cnt = 0;
  char host[1024];
  int querylen = 0;
  QueryData *q = (QueryData *) calloc(sizeof(QueryData),1);
  
  // Defaults
  q->options.nresults = 4;
  q->options.position = 0;
  q->options.engine_name = "googlev1";
  q->options.language = "en";
  q->options.header = 1;
  q->options.footer = 1;
  q->options.format = 0; // NONE

  process_options(argc,argv,q);
  
  if(!(q->options.urls | q->options.prime |
       q->options.snippets | q->options.ads |
       q->options.titles)) { 
    q->options.urls = 1; // Always default to fetching urls 
  }
  
  return(plugin_googlev1(&q->options));
}
