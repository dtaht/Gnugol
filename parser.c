#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_STR 1024
#define MAX_LINKS 10

char parse(char *s, char *links[], char *snippets[]) {
    char *pstart;
    char *pend;
    char *pprev;
    char *p;
    int   n1,n2;

    /* Parse LNK...END section -----------------------------------*/

    pstart = strstr(s, "LNK\n");
    if (pstart == NULL) {
	return -1;
    }
    pend = strstr(s, "END\n");
    if (pend == NULL) {
	return -1;
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
    } while((pprev < pend) && (n1 < MAX_LINKS));

    if (n1 >= MAX_LINKS)
	return -1;

    /* Parse SNP...END section -----------------------------------*/

    pstart = strstr(p, "SNP\n");
    if (pstart == NULL) {
	return -1;
    }
    pend = strstr(p, "END\n");
    if (pend == NULL) {
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
    } while((pprev < pend) && (n2 < MAX_LINKS));

    if (n2 >= MAX_LINKS)
	return -1;

    if (n1 != n2)
	return -1;

    return n1;
}

int main() {
    char answer[MAX_STR];
    char *links[MAX_LINKS];
    char *snippets[MAX_LINKS];
    int  nlinks, i;

    strcpy(answer,"LNK\nhttp://www.teklibre.com\nhttp://www.lwn.net\nhttp://www.slashdot.org\nhttp://a.very.busted.url\ngnugol://test+query\nEND\nSNP\nTeklibre is about to become the biggest albatross around David's head\nSlashdot Rules\nThis is a very busted url\nOne day we'll embed search right in the browser\nExtra snippet\nEND\n");

    nlinks = parse(answer, links, snippets);
    if (nlinks == -1) {
	printf("Error!\n");
	exit(1);
    }
	  
    for(i=0; i<nlinks; i++) 
	printf("[%d] [%s] [%s]\n", i, links[i], snippets[i]);

    return 0;
}
