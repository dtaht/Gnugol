#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "query.h"
#define DEBUG 1
#if defined (DEBUG)
#define log(a,b) printf(a,b);
#endif

// FIXME, this needed to be more free format
// See snippet
// And we need to init the 

/* 
int parse_section(QueryData *q,char *proto) {
    pstart = strstr(s, "LNK\n");
    if (pstart == NULL) {
	log("blew up looking for LNK\n","");
	return -1;
    }
    pend = strstr(s, "END\n");
    if (pend == NULL) {
	log("blew up looking for END\n","");
	return -2;
    }

    p = pstart; // BAD!
    pprev = strsep(&p, "\n");
    n1 = 0;
    do {
	pprev = strsep(&p, "\n");
	if (pprev != pend) {
	    q->links[n1] = pprev;
	    n1++;
	}
    } while((pprev < pend) && (n1 < MAX_ENTRIES));

    if (n1 > MAX_ENTRIES) {
	log("Overran entries table %d \n",n1);
	// return -3;
     }
    }
}
*/

int answer_parse(QueryData *q) {
  char *s = q->query;
    char *pstart = q->query; // Answer?
    char *pend;
    char *pprev;
    char *p;
    int   n1,n2,n3;

    /* Parse LNK...END section -----------------------------------*/

    if(q->options.urls) {
    pstart = strstr(s, "LNK\n");
    if (pstart == NULL) {
	log("blew up looking for LNK\n","");
	return -1;
    }
    pend = strstr(s, "END\n");
    if (pend == NULL) {
	log("blew up looking for END\n","");
	return -2;
    }

    p = pstart; // BAD!
    pprev = strsep(&p, "\n");
    n1 = 0;
    do {
	pprev = strsep(&p, "\n");
	if (pprev != pend) {
	    q->links[n1] = pprev;
	    n1++;
	}
    } while((pprev < pend) && (n1 < MAX_ENTRIES));

    if (n1 > MAX_ENTRIES) {
	log("Overran entries table %d \n",n1);
	// return -3;
     }
    /* Parse SNP...END section -----------------------------------*/
    }
    if(q->options.snippets) {
    pstart = strstr(p, "SNP\n");
    if (pstart == NULL) {
	log("No Snippet\n","");
	//return -1;
    }
    pend = strstr(p, "END\n");
    if (pend == NULL) {
	log("No End\n","");
	return -1;
    }

    p = pstart;
    pprev = strsep(&p, "\n");
    n2 = 0;
    do {
	pprev = strsep(&p, "\n");
	if (pprev != pend) {
	    q->snippets[n2] = pprev;
	    n2++;
	}
    } while((pprev < pend) && (n2 < MAX_ENTRIES));

//    if (n2 >= MAX_ENTRIES)
//	return -1;
    }

    if(q->options.titles) {
    pstart = strstr(p, "TLE\n");
    if (pstart == NULL) {
	log("No titles\n","");
	//return -1;
    }
    pend = strstr(p, "END\n");
    if (pend == NULL) {
	log("No End\n","");
	return -1;
    }

    p = pstart;
    pprev = strsep(&p, "\n");
    n3 = 0;
    do {
	pprev = strsep(&p, "\n");
	if (pprev != pend) {
	    q->titles[n3] = pprev;
	    n3++;
	}
    } while((pprev < pend) && (n3 < MAX_ENTRIES));

//    if (n2 >= MAX_ENTRIES)
//	return -1;
    }


    if (n1 != n2 && (q->options.snippets & q->options.urls)) {
	log("More q->snippets than q->urls",""); // FIXME
	// return -5;
    }

    return n1;
}

#ifdef TEST_PARSER

int main() {
    int  nlinks, i;
    QueryData q;
    
    strcpy(q.answer,"LNK\nhttp://www.teklibre.com\nhttp://www.lwn.net\nhttp://www.slashdot.org\nhttp://a.very.busted.url\ngnugol://test+query\nEND\nSNP\nTeklibre is about to become the biggest albatross around David's head\nLwn Rocks\nSlashdot Rules\nThis is a very busted url\nOne day we'll embed search right in the browser\nEND\n");
    nlinks = answer_parse(&q);
    if (nlinks == -1) {
	printf("Error!\n");
	exit(1);
    }
	  
    for(i=0; i<nlinks; i++) 
      printf("[%d] [%s] [%s]\n", i, q.links[i], q.snippets[i]); // SYNTAX HELL

    return 0;
}

#endif
