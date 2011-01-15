/*
Copyright (C) 2011 Michael D. Täht
Copyright (C) 2011 Sean Conner

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see http://www.gnu.org/licenses/
 */

/* Command line client for gnugol */

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <locale.h>
#include <assert.h>
#include <getopt.h>
#include <iconv.h>

#include "nodelist.h"
#include "query.h"
#include "formats.h"
#include "gnugol_engines.h"
#include "utf8_urlencode.h"

struct  output_types {
  const enum gnugol_formatter id;
  const char *const desc;
};

// FIXME: Verify differences between ikiwiki and media wiki format

List c_engines;

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
  { FORMATLISP, "lisp" },
  { 0, NULL },
  };

int usage (char *err) {
  if(err) fprintf(stderr,"%s\n",err);
  printf("gnugol [options] keywords to search for\n");
  printf(
	 "-e --engine        [bing|google|dummy]\n"
	 "-o --output        [html|json|org|mdwn|wiki|text|term|ssml|textile|raw]\n"
	 "-C --charset cs    character set used locally\n"
	 "-n --nresults      number of results to fetch\n"
	 "-p --position      start of results to fetch\n"
	 "-s --snippets  0|1 disable/enable snippets\n"
	 "-t --titles    0|1 disable/enable titles\n"
	 "-u --urls      0|1 disable/enable urls\n"
	 "-H --header    0|1 disable/enable output header\n"
	 "-F --footer    0|1 disable/enable output footer\n"
	 "-i --indent    X   result indentation level\n"
	 "-S --safe    0|1|2 [off|moderate|active] result filtering\n"
	 "-D --debug     X   debug output level\n"
	 "-U --escaped       input is already url escaped\n"
	 "-l --language-in   [en|es|fr|nl or other 2 char locale]\n"
	 "-L --language-out  [en|es|fr|nl or other 2 char locale]\n"
	 "-v --verbose       provide more verbose insight\n"
	 "-h --help          this message\n"
	 "-a --about         [credits|copyright|license|source|config|manual|stats]\n"

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
	 "-d --desc      0|1 disable/enable descriptions\n"
	 "-c --cache     serve only results from cache(s)\n"
	 "-f --force     force a new query, even if cached\n"
	 "-I --input     [filename] input from a file\n"
	 "-5 --offline   store up query for later\n"
	 "-r --reverse   reverse the list\n"
#endif

#ifdef WHATABOUTTHESE
	 "-C --config\n"
	 "-g --plugin\n"
	 "   --source\n"
#endif
  );

  exit(-1);
}

static const struct option long_options[] = {
  { "about"		, no_argument		, NULL , 'a' } ,
  { "charset"		, required_argument	, NULL , 'C' } ,
  { "debug"		, required_argument	, NULL , 'D' } ,
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
  { "safe"		, required_argument    	, NULL , 'S' } ,
  { "titles"		, required_argument	, NULL , 't' } ,
  { "urls"		, required_argument	, NULL , 'u' } ,
  { "desc"		, required_argument	, NULL , 'd' } ,
  { "escaped"      	, no_argument		, NULL , 'U' } ,
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
  { "word-break"	, no_argument		, NULL , 'B' } ,
  { "config"		, no_argument		, NULL , 'C' } ,
  { "plugin"		, no_argument		, NULL , 'g' } ,
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

#define BOOLOPT(OPTION) OPTION = (strtoul(optarg,NULL,10) & 1)
#ifdef HAVE_GNUGOLD
#  define QSTRING "aC:d:D:e:F:H:hi:l:L:n:o:p:s:S:t:u:Uvb46mPRST"
#else
#  define QSTRING "aC:d:D:e:F:H:hi:l:L:n:o:p:s:S:t:u:Uv"
#endif

int process_options(int argc, char **argv, QueryOptions_t *o)
{
  int          option_index;
  int          opt;
  GnuGolEngine engine;

  option_index = 0;
  opt          = 0;
  optind       = 1;

  while(true)
  {
    opt = getopt_long(argc, argv,
		      QSTRING,
		      long_options, &option_index);
    if(opt == -1) break;

    switch (opt)
    {
      case 'a': o->about = 1;  break;
      case 'C': o->charset = optarg; break;
      case 'd': o->desc = strtoul(optarg,NULL,10); break;
      case 'D': o->debug = strtoul(optarg,NULL,10); break;
      case 'e':
           o->engine = 1;
           engine = gnugol_engine_load(optarg);
           if (engine == NULL)
             fprintf(stderr,"engine %s not supported\n",optarg);
           else
             ListAddTail(&c_engines,&engine->node);
           break;
      case 'F': BOOLOPT(o->footer); break;
      case 'H': o->header = strtoul(optarg,NULL,10); break;
      case 'h':
      case '?': usage(NULL); break;
      case 'i': o->indent = strtoul(optarg,NULL,10); break;
      case 'l': strcpy(o->input_language,optarg); break;
      case 'L': strcpy(o->output_language,optarg); break;
      case 'n': o->nresults = strtoul(optarg,NULL,10); break;
      case 'o':
           for(int i = 0; output_type[i].desc != NULL; i++)
	    if(strcmp(output_type[i].desc,optarg) == 0)
	      o->format = output_type[i].id;
           break;
      case 'p': o->position = strtoul(optarg,NULL,10); break;
      case 's': o->snippets = strtoul(optarg,NULL,10); break;
      case 'S': o->safe = strtoul(optarg,NULL,10); break;
      case 't': o->titles = strtoul(optarg,NULL,10); break;
      case 'u': o->urls = strtoul(optarg,NULL,10); break;
      case 'U': o->url_escape = 1; break;
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
      /* source missing */
#endif

      default: fprintf(stderr,"%d",opt); usage("Invalid option"); break;
    }
  }

  return optind;
}

int finish_setup(QueryOptions_t *o,int idx,int argc,char **argv)
{
  GnuGolEngine engine;
  char    string[MAX_MTU];
  size_t  querylen = 0;
  int     i;


  if (o->charset != NULL)
  {
    o->icin = iconv_open("UTF-8",o->charset);
    if (o->icin == (iconv_t)-1)
      return errno;

    o->icout = iconv_open(o->charset,"UTF-8");
    if (o->icout == (iconv_t)-1)
      return errno;
  }

  string[0] = '\0';

  if (!o->about && ListEmpty(&c_engines))
  {
    engine = gnugol_engine_load(o->engine_name);
    if (engine == NULL)
    {
      fprintf(stderr,"default engine not found!  Panic!\n");
      return ENOENT;
    }

    ListAddTail(&c_engines,&engine->node);
  }

  if (o->about)
  {
    engine = gnugol_engine_load("credits");
    if (engine == NULL)
    {
      fprintf(stderr,"default engine not found!  Panic!\n");
      return ENOENT;
    }

    ListAddTail(&c_engines,&engine->node);
  }

  for(i = idx; i < argc; i++)
  {
    size_t  arginlen;
    size_t  argoutlen;

    arginlen  = strlen(argv[i]);
    argoutlen = arginlen * 4;

    char   tmpbuf[argoutlen + 1];	/* guess at a good size */
    char   *tin  = argv[i];
    char   *tout = tmpbuf;
    char   *word;
    size_t  convlen;

    if (o->charset != NULL)
    {
      convlen = iconv(o->icin,&tin,&arginlen,&tout,&argoutlen);
      if (convlen == (size_t)-1)
        return errno;

      assert(tout != tmpbuf);
      *tout = '\0';
      word  = tmpbuf;
    }
    else
      word = argv[i];

    if((querylen += (strlen(word)+1) > MAX_MTU - 80))
    {
      fprintf(stderr,"Too many words in query, try something smaller\n");
      return EINVAL;
    }

    /* FIXME: Although I did a length check above it could be cleaner here */

  if(!o->url_escape)
    {
      strcat(string,word);
      if(i+1 < argc) strcat(string," ");
    }
    else
    {
       strcat(string,word);
      if(i+1 < argc) strcat(string,"+");
    }
  }

  if(!o->url_escape) {
	  url_escape_utf8(o->keywords,string);
	  o->url_escape = 1;
  } else {
	  strcpy(o->keywords,string);
  }
  if(o->debug > 0) print_enabled_options(o, stderr);
  if(!(o->urls | o->snippets | o->ads | o->titles)) o->urls = 1;
  if(o->about) {
    o->engine_name = "credits";
    o->header_str = "About: ";
  }
  return 0;
}

static void gnugol_default_language (QueryOptions_t *q) {
	// Get this from the locale? What's portable?
	// FIXME: We always want utf-8

	char *lang = getenv("LANG");
	if(lang != NULL) {
		strcpy(q->input_language,lang);
	} else {
		strcpy(q->input_language,"en_US.utf8");
	}
}

static void gnugol_default_QueryOptions(QueryOptions_t *q) {
	q->nresults = 5;
	q->position = 0;
	q->urls = 1;
	q->snippets = 1;
	q->titles = 1;
	q->engine_name = "google";
	q->header = 1;
	q->footer = 1;
	q->format = FORMATDEFAULT; // ORG
	q->indent = -1; // None
	q->safe = 1; // Moderate
	gnugol_default_language(q);
}

void process_environ(char *program,QueryOptions_t *query)
{
  char   *opt;
  size_t  optlen;

  assert(program != NULL);
  assert(query   != NULL);

  opt = getenv("GNUGOL_OPTS");
  if (opt == NULL) return;
  optlen = strlen(opt) + 1;

  char    optcopy[optlen];
  char   *argv[optlen + 1];
  size_t  argc;
  char   *p;

  memcpy(optcopy,opt,optlen);
  p       = optcopy;
  argv[0] = program;

  for (argc = 1 ; ; )
  {
    argv[argc] = strtok(p," \t\v\r\n");
    if (argv[argc] == NULL) break;
    argc++;
    p = NULL;
  }

  process_options(argc,argv,query);
}

int main(int argc, char **argv) {
  int            result;
  int            words;
  QueryOptions_t master;
  QueryOptions_t q;
  GnuGolEngine   engine;

  /*------------------------------------------------------------------------
  ; sorted data in this program is collated in the C locale.  Set that here
  ; to make sure the program returns sane data when calling bsearch().
  ;-----------------------------------------------------------------------*/

  setlocale(LC_COLLATE,"C");

  ListInit(&c_engines);

  gnugol_init_QueryOptions(&master);
  gnugol_default_QueryOptions(&master);

  process_environ(argv[0],&master);
  words = process_options(argc,argv,&master);

  if (finish_setup(&master,words,argc,argv) != 0)
    return EXIT_FAILURE;

  assert(!ListEmpty(&c_engines));

  for(
        engine = (GnuGolEngine)ListGetHead(&c_engines);
        NodeValid(&engine->node);
        engine = (GnuGolEngine)NodeNext(&engine->node)
      )
  {
    q      = master;
    result = gnugol_engine_query(engine,&q);

    if(q.returned_results > 0)
    {
      if (q.charset)
      {
        char    outbuffer[(q.out.len * 4) + 1];
        size_t  convlen;
        char   *tin  = q.out.s;
        size_t  sin  = q.out.len;
        char   *tout = outbuffer;
        size_t  sout = sizeof(outbuffer);

        convlen = iconv(q.icout,&tin,&sin,&tout,&sout);
        if (convlen == (size_t)-1)
          return EXIT_FAILURE;

        assert(tout != outbuffer);
        *tout = '\0';
        printf("%s",outbuffer);
      }
      else
        printf("%s",q.out.s);
    }

    if(result < 0 || q.debug > 5) {
      fprintf(stderr,"Errors: %s\nWarnings:%s\n",q.err.s,q.wrn.s);
    }

    if(q.debug > 10) {
      fprintf(stderr,"out len = %d\n size = %d, Contents = %s\n",q.out.len, q.out.size, q.out.s);
      fprintf(stderr,"wrn len = %d\n size = %d, Contents = %s\n",q.wrn.len, q.wrn.size, q.wrn.s);
      fprintf(stderr,"err len = %d\n size = %d, Contents  = %s\n",q.err.len, q.err.size, q.err.s);
    }
    /*gnugol_free_QueryOptions(&q);*/
  }

  for(
       engine = (GnuGolEngine)ListRemTail(&c_engines);
       NodeValid(&engine->node);
       engine = (GnuGolEngine)ListRemTail(&c_engines)
     )
  {
    gnugol_engine_unload(engine);
  }

  return(0);
}
