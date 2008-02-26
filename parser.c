#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "query.h"
#define DEBUG 1
#if defined (DEBUG)
#define log(a,b) printf(a,b);
#endif

int answer_parse(char *s, QueryData *q) {
    char *pstart;
    char *pend;
    char *pprev;
    char *p;
    int   n1,n2;
    char **links = q->links;
    char **snippets = q->snippets;

    /* Parse LNK...END section -----------------------------------*/

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

    p = pstart;
    pprev = strsep(&p, "\n");
    n1 = 0;
    do {
	pprev = strsep(&p, "\n");
	if (pprev != pend) {
	    links[n1] = pprev;
	    n1++;
	}
    } while((pprev < pend) && (n1 < MAX_ENTRIES));

    if (n1 >= MAX_ENTRIES) {
	log("Overrand entries table \n","");
	// return -3;
     }
    /* Parse SNP...END section -----------------------------------*/

    pstart = strstr(p, "SNP\n");
    if (pstart == NULL) {
	log("No Snippet\n","");
	return -1;
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
	    snippets[n2] = pprev;
	    n2++;
	}
    } while((pprev < pend) && (n2 < MAX_ENTRIES));

//    if (n2 >= MAX_ENTRIES)
//	return -1;

    if (n1 != n2) {
	log("More snippets than links","");
	return -5;
    }

    return n1;
}

#ifdef TEST_PARSER

int main() {
    char answer[MAX_MTU];
    char *links[MAX_ENTRIES];
    char *snippets[MAX_ENTRIES];
    int  nlinks, i;
    QueryData q;

    strcpy(answer,"LNK\nhttp://www.teklibre.com\nhttp://www.lwn.net\nhttp://www.slashdot.org\nhttp://a.very.busted.url\ngnugol://test+query\nEND\nSNP\nTeklibre is about to become the biggest albatross around David's head\nLwn Rocks\nSlashdot Rules\nThis is a very busted url\nOne day we'll embed search right in the browser\nEND\n");

    nlinks = answer_parse(answer, &q);
    if (nlinks == -1) {
	printf("Error!\n");
	exit(1);
    }
	  
    for(i=0; i<nlinks; i++) 
	printf("[%d] [%s] [%s]\n", i, links[i], snippets[i]);

    return 0;
}

#endif
