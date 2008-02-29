/* Command line client for gnugol *
/* This code is Copyright (C) 2008 by Michael David Taht 
   and released under the terms of the GNU AFFERO PUBLIC_LICENSE, version 3 
   for details of this license see the COPYRIGHT file */

#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include "query.h"

/* FIXME - look over wget options for useful stuff

*/

int usage () {
 printf("gnugol [options] keywords - no options are available and only a keyword string is taken\n");
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
 printf("--config"); printf(" --verbose"); printf(" --copyright"); printf(" --license\n"); 
}

main(int argc, char **argv) {
	char myquery[MAX_MTU];
	QueryData q;
	q.query = myquery;
	int i;
	if(argc == 2) {
	  if(strlen(argv[1]) < MAX_MTU-1) {
		strcpy(myquery,argv[1]); // FIXME, check length
	  } else {
	    printf("query too big\n");
	  }
	} else {
	  usage();
	  exit(-1);
	}
#ifdef DUMMY_CLIENT
	int cnt = query_main(&q,"localhost");
#else
	int cnt = query_main(&q,NULL);
#endif
	for (i = 0; i <= cnt-1; i++) {
	  printf("<a href=%s>%s</a><br>", q.links[i], q.snippets[i]); 
	}
	printf("\n");
 }

