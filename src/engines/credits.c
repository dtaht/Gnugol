/* This engine implements contributor credits, licensing and copyright information,  
   and other misc configuration parameters internal to gnugol */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <curl/curl.h>
#include "query.h"
#include "utf8.h"
#include "handy.h"
#include "formats.h"
#include "gnugol_engines.h"

#ifndef __GNUC__
#define __attribute__(x)
#endif

struct credits { 
  const char *catagory;
  const char *name;
  const char *url;
  const char *desc;
};

static const struct credits c[] = { 
  { "credits", "Dave Täht", "http://www.taht.net/", "Inspiration... and perspiration" }, 
  { "credits", "Craig Guessford", "http://www.aftertheflood.com/", "Gnugol logo and related art" }, 
  { "credits", "Sean Conner", "http://boston.conman.org/", "DLLs, UTF-8 improvements" }, 
  { "credits", "Brian Clapper", "http://www.clapper.org/bmc/", "OSX, textile, mdwn support" }, 
  { "copyright", "&copy; Sean Conner 2010", "http://boston.conman.org/", "Portions contributed by Sean Connor" },
  { "copyright", "© Michael D. Taht 2010", "http://www.taht.net/", 
    "The bulk of gnugol is under the AGPLv3. See --about license for more details" },
  { "license", "Gnugol's Licensing", GNUGOL_SITE, "FIXME: Manifesto to be made here" },
  { "license", "GNU Affero General Public License, Version 3", "http://www.gnu.org/licenses/agpl.html", 
"The GNU Affero General Public License is a free, copyleft license for software and other kinds of works, specifically designed to ensure cooperation with the community in the case of network server software.\n\n"
"The GNU Affero General Public License is designed specifically to ensure that the modified source code becomes available to the community. It requires the operator of a network server to provide the source code of the modified version running there to the users of that server. Therefore, public use of a modified version, on a publicly accessible server, gives the public access to the source code of the modified version.\n\n (For the complete license, visit the web site)\n\n" },
  { "license", "GNU General Public License", "http://www.gnu.org/licenses/gpl.html", "" },
  { "license", "GNU General Lesser General Public License", "http://www.gnu.org/licenses/lgpl.html", "" },
  { "license", "Free Beer License", "http://en.wikipedia.org/wiki/Beerware", "" },
  { "source",  "Gnugol Source Code", "https://github.com/dtaht/Gnugol.git", "" },
  { "config",  "Gnugol Configuration", "https://github.com/dtaht/Gnugol.git", "At present gnugol can only be configured via command line options. There will be a ~/.gnugol/config someday, in json format. " },
  { "manual",  "Gnugol Manual", GNUGOL_SITE, "Sorry, no manual yet. See the web site for some tips or try --help" },
  { "jwz", "jwz", "http://www.jwz.org/blog", "I share jwz's preference for green on black screens. His gruntle columns kept me sane in a darker era of web development, and I always loved the subversive element of the about:jwz parameter of Mozilla in an otherwise bland, corporatized world." },
  { "quotes", "jwz", "http://www.jwz.org/blog", "Some people, when confronted with a problem, think 'I know, I'll use regular expressions.' Now they have two problems."},
  { "gnugol","gnugol",GNUGOL_SITE,"BEHOLD. THE WORLD WAS GREEN ON BLACK AND IT WAS GOOD." },  
  { NULL,NULL, NULL, NULL },
};

struct cat_map { 
  const char *catagory;
  const char *desc;
};

static const struct cat_map cmap[] = { 
 { "license","Gnugol Licenses" },
 { "credits","Gnugol Contributor Credits" },
 { "source","Gnugol Source availability" },
 { "copyright","Gnugol Copyrights" },
 { "manual", "Gnugol Manual" },
 { "all", "Gnugol Internal Information" },
 { NULL, NULL },
};

int GNUGOL_DECLARE_ENGINE(setup,credits) (QueryOptions_t *q __attribute__((unused)))
{
  return 0;
}

// FIXME: Add keywords of:
// license, credits, source, config, stats, errors, warnings, all
// FIXME: Figure out how to get the build commit, build date, etc
//        out of git
// FIXME: Add keyword escapes elsewhere, gnugol:whatever
// FIXME: Use explicit headers! not! the keyword string and simplify
// FIXME: Create better formatted output, with headers
// FIXME: allow multiple keywords

int GNUGOL_DECLARE_ENGINE(search,credits) (QueryOptions_t *q) {
  char tempstr[20];  
  int header_no = -1;
  q->indent = 2;
  if(strcmp(q->keywords,"all") == 0 || q->keywords[0] == '\0') {
    gnugol_header_out(q);
    for(int i = 0; c[i].name != NULL; i++) {
      for(int j = 0; cmap[j].catagory != NULL; j++) {
	if(strcmp(cmap[j].catagory,c[i].catagory) == 0) {
	  if(header_no != j) {
	    q->indent -= 1;
	    gnugol_result_out(q, "", "", cmap[j].desc );
	    header_no = j;
	    q->indent += 1;
	    gnugol_result_out(q,c[i].url,c[i].name,c[i].desc);
	  } else {
	    gnugol_result_out(q,c[i].url,c[i].name,c[i].desc);
	  }
	}
      }
    }
    gnugol_footer_out(q);
  } else {
    if(q->keywords[0] != '\0') {
    gnugol_header_out(q);
    for(int i = 0; c[i].name != NULL; i++) {
      if(strcmp(c[i].catagory,q->keywords) == 0) {
	gnugol_result_out(q,c[i].url,c[i].name,c[i].desc);
      }
    }
    gnugol_footer_out(q);
    }
  }
  return 0;
}
