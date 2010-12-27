/* This engine implements contributor credits. */

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
  const char *name;
  const char *url;
  const char *desc;
};

// These are in reverse alphabetical order

static const struct credits c[] = { 
  { "Dave Täht", "http://www.taht.net", "Inspiration... and perspiration" }, 
  { "Craig Guessford", "http://www.aftertheflood.com", "Gnugol logo and related art" }, 
  { "Sean Conner", "http://boston.conman.org", "DLLs, UTF-8 improvements" }, 
  { "Brian Clapper", "http://www.clapper.org/bmc", "OSX, textile, mdwn support" }, 
  { NULL, NULL, NULL },
};

static const struct credits copyrights[] = {
  { "© Michael D. Taht 2010", "http://www.taht.net", "" },
  { "© Sean Conner 2010", "http://", "" },
  { NULL, NULL, NULL },
};

static const struct credits licenses[] = {
  { "Overview", "http://gnugol.taht.net/", "FIXME: Discussion to be made here" },
  { "GNU Affero General Public License, Version 3", "http://www.gnu.org/licenses/agpl.html", 
"The GNU Affero General Public License is a free, copyleft license for software and other kinds of works, specifically designed to ensure cooperation with the community in the case of network server software.\n\n"
"The GNU Affero General Public License is designed specifically to ensure that the modified source code becomes available to the community. It requires the operator of a network server to provide the source code of the modified version running there to the users of that server. Therefore, public use of a modified version, on a publicly accessible server, gives the public access to the source code of the modified version.\n\n (For the complete license, visit the web site)\n\n" },
  { "GNU General Public License", "http://www.gnu.org/licenses/gpl.html", "" },
  { "GNU General Lesser General Public License", "http://www.gnu.org/licenses/lgpl.html", "" },
  { "Free Beer License", "http://en.wikipedia.org/wiki/Beerware", "" },
  { NULL, NULL, NULL },
};

int GNUGOL_DECLARE_ENGINE(setup,credits) (QueryOptions_t *q __attribute__((unused)))
{
  return 0;
}

// FIXME: Add keywords of:
// license, about, credits, source, config, stats, errors, warnings, all
// FIXME: Add keyword escapes elsewhere, gnugol:whatever

int GNUGOL_DECLARE_ENGINE(search,credits) (QueryOptions_t *q) {
  strcpy(q->keywords,"Gnugol Contributor Credits");
  gnugol_header_out(q);
  for(int i = 0; c[i].name != NULL; i++) {
    gnugol_result_out(q,c[i].url,c[i].name,c[i].desc,NULL);
  }
  gnugol_footer_out(q);
  return 0;
}
