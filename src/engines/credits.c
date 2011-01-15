/*
Copyright (C) 2011 Michael D. Täht

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

/* This engine implements contributor credits, licensing and
   copyright, and other misc configuration parameters internal to gnugol */

#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <jansson.h>
#include <curl/curl.h>
#include <iconv.h>
#include "query.h"
#include "utf8.h"
#include "handy.h"
#include "formats.h"
#include "gnugol_engines.h"

#ifdef __APPLE__
#  define SO_EXT	"dylib"
#endif

#if defined(__linux__) || defined(__FreeBSD__)
#  define SO_EXT	"so"
#endif

struct credits {
  const char *catagory;
  const char *name;
  const char *url;
  const char *desc;
};

const char description[] = "Much of the internal documentation of gnugol is implemented by the \"credits\" engine.";

static const struct credits c[] = {
  { "credits", "Dave Täht", "http://www.taht.net/", "Inspiration... and perspiration" },
  { "credits", "Craig Guessford", "http://www.aftertheflood.com/", "Gnugol logo and related art" },
  { "credits", "Sean Conner", "http://boston.conman.org/", "DLLs, UTF-8 improvements, cleanups, bug fixes" },
  { "credits", "Brian Clapper", "http://www.clapper.org/bmc/", "OSX, textile, mdwn support" },
  { "credits", "David Rowe", "http://www.rowetel.com/", "Original scraper/fpipe code, moral support" },
  { "credits", "Nick Mainsbridge", "http://deprogram.net/", "The name, 'gnugol', and the backing music" },
  { "copyright", "© 2010,2011 Sean Conner", "http://boston.conman.org/", "Portions contributed by Sean Conner" },
  { "copyright", "© 2010,2011 Michael D. Täht", "http://www.taht.net/",
    "The bulk of gnugol is under the AGPLv3. See --about license for more details" },
  { "license", "Gnugol's Licensing", GNUGOL_SITE, "" },
  { "license", "GNU Affero General Public License, Version 3", "http://www.gnu.org/licenses/agpl.html",
"The GNU Affero General Public License is a free, copyleft license for software and other kinds of works, specifically designed to ensure cooperation with the community in the case of network server software.\n\n"
"The GNU Affero General Public License is designed specifically to ensure that the modified source code becomes available to the community. It requires the operator of a network server to provide the source code of the modified version running there to the users of that server. Therefore, public use of a modified version, on a publicly accessible server, gives the public access to the source code of the modified version.\n\n (For the complete license, visit the web site)\n\n" },
  { "license", "GNU General Public License", "http://www.gnu.org/licenses/gpl.html", "" },
  { "license", "GNU General Lesser General Public License", "http://www.gnu.org/licenses/lgpl.html", "" },
  { "license", "Free Beer License", "http://en.wikipedia.org/wiki/Beerware", "" },
  { "source",  "Gnugol Source Code via git", "https://github.com/dtaht/Gnugol.git", "" },
  { "source",  "Mirror sites", GNUGOL_SITE, "The main site has a list of mirrors" },
  { "config",  "Gnugol Configuration", "https://github.com/dtaht/Gnugol.git", "At present gnugol can only be configured via command line options. There will be a ~/.gnugol/config someday, in json format. " },
  { "manual",  "Gnugol Manual", GNUGOL_SITE, "Sorry, no manual yet. See the web site for some tips or try --help" },
  { "keys",  "Gnugol API Key", GNUGOL_SITE, "No license key for gnugol is (or will ever be) required. That said, certain other services do require a license key, and I'd like to be able to search those, so I have made allowances for search engines that require one." },
  { "keys",  "Google API Key", "http://code.google.com/apis/ajaxsearch/documentation/reference.html", "A key is not required, but desirable. If you get/have one, put it in ~/.googlekey to use." },
  { "keys",  "Bing API Key", "http://www.bing.com/developers/createapp.aspx", "Get a bing key from this url and put it in a file called ~/.bingkey." },
  { "keys",  "Wikipedia", GNUGOL_SITE, "No json API key is required from the wikimedia foundation (thankfully). Unfortunately, the wikipedia search engine as written thus far, doesn't work. :(" },
  { "jwz", "jwz", "http://www.jwz.org/gruntle/design.html", "I share jwz's preference for green on black screens. His gruntle columns kept me sane in a darker era of web development, and I always loved the subversive element of the about:jwz parameter of Mozilla in an otherwise bland, corporatized world." },
  { "rms", "rms", "http://www.stallman.org", "If he didn't exist, we'd have had to invent him." },
  { "quotes", "esr", "http://esr.ibiblio.org/", "With enough eyeballs, all bugs are shallow." },
  { "quotes", "jwz", "http://www.jwz.org/blog/", "Some people, when confronted with a problem, think 'I know, I'll use regular expressions.' Now they have two problems."},
  { "quotes", "mdt", "http://nex-6.taht.net/", "Some people, when confronted with a problem, think 'I know, I'll use IPv6.' Now they have two problems."},
  { "gnugol","gnugol",GNUGOL_SITE,"BEHOLD: THE WORLD IS GREEN ON BLACK AND WITHOUT IMAGES, AND IT IS GOOD." },
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
 { "keys", "Gnugol API Keys" },
 { "all", "Gnugol Internal Information" },
 { NULL, NULL },
};

static int search_engines(QueryOptions_t *q)
{
  size_t  extlen;
  DIR    *dir;

  dir = opendir(GNUGOL_SHAREDLIBDIR);
  if (dir == NULL)
  {
    /* error */
    return 0;
  }

  extlen = strlen(SO_EXT);

  while(1)
  {
    GnuGolEngine  engine;
    struct dirent entry;
    struct dirent *pentry;
    size_t         len;
    char           name[BUFSIZ];
    int            rc;

    rc = readdir_r(dir,&entry,&pentry);
    if (rc != 0)
    {
      /* error */
      break;
    }

    if (pentry == NULL) break;
    len = strlen(entry.d_name);
    if (len <= extlen) continue;
    if (strcmp(&entry.d_name[len-extlen],SO_EXT) != 0) continue;
    memcpy(name,entry.d_name,(len - extlen) - 1);
    name[(len - extlen) - 1] = '\0';

    if((engine = gnugol_engine_load(name)) != NULL)
    {
    gnugol_result_out(q,NULL,engine->name,engine->description);
    gnugol_engine_unload(engine);
    }
  }

  gnugol_result_out(
  	q,
  	"http://gnugol.taht.net/bugs.html",
  	"(more)",
  	"More engines would be AWESOME! Citeseer and Gmane especially! See list on the web site for more...."
  );
  closedir(dir);
  return 0;
}

// FIXME: Add keywords of: source, config, stats, errors, warnings
// FIXME: Figure out how to get the build commit, build date, etc
//        out of git
// FIXME: Add keyword escapes elsewhere, gnugol:whatever
// FIXME: Use explicit headers! not! the keyword string and simplify
// FIXME: Create better formatted output, with headers
// FIXME: allow multiple keywords

int search(QueryOptions_t *q) {
  int header_no = -1;
  q->indent = 2;

  if((strcmp("all",q->keywords) == 0) || q->keywords[0] == '\0') {
    q->indent -= 1;
    gnugol_result_out(q,"","","Gnugol's supported search engines");
    q->indent += 1;
    gnugol_header_out(q);
    search_engines(q);
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
  }
  else if (strcmp("engines",q->keywords) == 0)
  {
    gnugol_header_out(q);
    search_engines(q);
    gnugol_footer_out(q);
  }
  else
  {
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
