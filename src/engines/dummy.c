/* This engine implements dummy layer - useful for debugging the 
   formatting engine */

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

// turn quotes back into quotes and other utf-8 stuff
// FIXME: Error outs cause a memory leak from "root"
// FIXME: do fuller error checking 

// Some Fuzzed inputs
// Maybe back off the number of results when we overflow the buffer
// Arguably this should obey other gnugol options (nresults, position) 
// at some point
// More Test cases for bad inputs should go here:

const char description[] = "The dummy engine is used for generating test/malformed data for each of the output formatters.";

int search(QueryOptions_t *q) {
  if(q->debug) GNUGOL_OUTW(q,"trying url: %s", q->querystr); 
  if(q->debug) GNUGOL_OUTW(q,"trying keywords: %s", q->keywords); 
  q->indent = 1;
  gnugol_header_out(q);
  gnugol_result_out(q,GNUGOL_SITE "bugs.html", "Gnugol Engine Tests", "Some tests of the formatter");
  q->indent = 2;
  gnugol_result_out(q,GNUGOL_SITE,"Gnugol - Command line Search","This is a dummy query");
  gnugol_result_out(q,"http://www.google.com","“Google Rocks”","google is the source of many good things.");
  gnugol_result_out(q,"http://cryptolisting.taht.net","Cryptolisting - Making STARTTLS more common...  ","<b>This</b> has <i>, well (had, in some cases) html</i>.");
  gnugol_result_out(q,"http://www.taht.net","Dave's web site", "Dave has a <i>web</i> site?");

  q->indent -= 1; gnugol_result_out(q,GNUGOL_SITE "bugs.html", "Stuff that needs to get escaped properly", "Some engines are going to need [[whatever]] escaped properly. Mailing lists in particular exhibit this problem.");
  q->indent += 1; GNUGOL_OUTW(q,"Tests: Formatting Escapes\n");
  gnugol_result_out(q,"http://www.lwn.net/][][[[]","Url with bad square bracket stuff", "");
  gnugol_result_out(q,"http://www.orgmode.org","[ORG] - My favorite mailing list", "Url with description with square brackets");
  gnugol_result_out(q,"http://www.lwn.net/][][[[]","[BAH!] - Humbug", "Url and description with bad square bracket stuff - it's not clear to me if you can even PUT square brackets in an url...");

  q->indent -= 1; gnugol_result_out(q,GNUGOL_SITE "bugs.html", "UTF-8", "");
  q->indent += 1; GNUGOL_OUTW(q,"Tests: UTF-8\n");
  gnugol_result_out(q,GNUGOL_SITE "bugs.html", "Unimplemented", "No tests for UTF-8 implemented yet");

  q->indent -= 1; gnugol_result_out(q,GNUGOL_SITE "bugs.html", "Bad HTML", "");
  q->indent += 1; GNUGOL_OUTW(q,"Tests: Bad HTML\n");
  gnugol_result_out(q,GNUGOL_SITE "bugs.html", "Unimplemented", "No tests for bad HTML implemented yet");
  
  q->indent -= 1; gnugol_result_out(q,GNUGOL_SITE "bugs.html", "Empty String", "");
  q->indent += 1; GNUGOL_OUTW(q,"Tests: Empty string\n");
  gnugol_result_out(q,"","There was no url in this test", "Empty url");
  gnugol_result_out(q,GNUGOL_SITE "bugs.html", "", "Empty title");
  gnugol_result_out(q,"", "", "Empty title and URL - all Empty coming up next - should not appear");
  gnugol_result_out(q,"", "", "");
  gnugol_result_out(q,GNUGOL_SITE "bugs.html", "End of empty string test", "Did it look right?");

  q->indent -= 1; gnugol_result_out(q,GNUGOL_SITE "bugs.html", "NULL Pointers", NULL);
  q->indent += 1; GNUGOL_OUTW(q,"Tests: Null PTR\n");
  gnugol_result_out(q,NULL, "There's a null url here", "NULL url");
  gnugol_result_out(q,GNUGOL_SITE "bugs.html", NULL, "NULL title");
  gnugol_result_out(q,NULL, NULL, "NULL title and URL - all NULLs coming up - should not appear");
  gnugol_result_out(q,NULL, NULL, NULL);
  gnugol_result_out(q,GNUGOL_SITE "bugs.html", "End of NULL ptr test", "No KaBOOM!? Awesome!");

  q->indent -= 1; gnugol_result_out(q,GNUGOL_SITE "bugs.html", "Wow - Gnugol survived", "It sure didn't when I first wrote these tests.");
  q->indent += 1; gnugol_result_out(q,GNUGOL_SITE "bugs.html", "Mere Survival Means NOTHING", "Now you need to go and check the quality of the output against the formatter for the output you were testing and make sure it looks correct.");

  gnugol_footer_out(q);
    // FIXME: Go recursive if we overflowed the buffer
    return 0;
}
