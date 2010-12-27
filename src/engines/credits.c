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
  { "Sean Connor", "http://boston.conman.org", "DLLs, UTF-8 improvements" }, 
  { "Brian Clapper", "http://www.clapper.org/bmc", "OSX, textile, mdwn support" }, 
  { NULL, NULL, NULL },
};


int GNUGOL_DECLARE_ENGINE(setup,credits) (QueryOptions_t *q __attribute__((unused)))
{
  return 0;
}

int GNUGOL_DECLARE_ENGINE(search,credits) (QueryOptions_t *q) {
  strcpy(q->keywords,"Gnugol Contributor Credits");
  gnugol_header_out(q);
  for(int i = 0; c[i].name != NULL; i++) {
    gnugol_result_out(q,c[i].url,c[i].name,c[i].desc,NULL);
  }
  gnugol_footer_out(q);
  return 0;
}
