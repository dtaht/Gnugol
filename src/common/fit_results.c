/* Fit the results into a buffer of size X */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "query.h"

int build_query(QueryData *q) {
  char *query = &q->query;
  if(query == NULL) { fprintf(stderr, "Null query data, bailing!\n"); return(-1); }
  strcpy(query,"GET ");
  if(q->options.urls) strcat(query, "LNK ");
  if(q->options.titles) strcat(query, "TLE ");
  if(q->options.snippets) strcat(query, "SNP ");
  if(q->options.ads) strcat(query, "ADS ");
  if(q->options.misc) strcat(query, "MSC ");
  strcat(query,"\n");
  strcat(query,q->keywords);
  fprintf(stderr,"Debug keywords:%s \n", q->keywords);
  strcat(query,"\nEND\n");
  return(0);
}

int
compress_results(QueryData *q, char *packet, int size) { // size will usually equal 1240
	int i;
	int nbytes = 0;
	// For now, don't compress
	for(i = 0; i < q->nresults-1 && nbytes < MAX_MTU - 40; i++) { 
	  nbytes += q->options.urls 	? 0 : q->nlinkbytes; 
	  nbytes += q->options.snippets ? 0 : q->nsnippetbytes;
	  nbytes += q->options.titles 	? 0 : q->ntitlebytes;
	  nbytes += q->options.ads 	? 0 : q->nadbytes;
	  nbytes += q->options.misc 	? 0 : q->nmiscbytes;
	}
	if(nbytes > 4*size) { 
		// odds are really good that we can't compress that many results, don't try
		return(-1); 
	}

	int resulttypes = q->options.urls + q->options.snippets + q->options.titles +
	  q->options.ads + q->options.misc;
	char *b = (char *) calloc (nbytes + 8*resulttypes,1);

	if(q->options.urls) {
		if(q->nurls > 0) {
			strcat(b,"LNK\n");
		for(i = 0; i < q->nurls; i++) {
			strcat(b,q->links[i]);	
		}
			strcat(b,"END\n");
		}
	}
	if(q->options.snippets) {
		if(q->nsnippets > 0) {
			strcat(b,"SNP\n");
		for(i = 0; i < q->nsnippets; i++) {
			strcat(b,q->snippets[i]);	
		}
			strcat(b,"END\n");
		}
	}
	if(q->options.titles) {
		if(q->ntitles > 0) {
			strcat(b,"TLE\n");
		for(i = 0; i < q->ntitles; i++) {
			strcat(b,q->titles[i]);	
		}
			strcat(b,"END\n");
		}
	}
	if(q->options.ads) {
		if(q->nads > 0) {
			strcat(b,"ADS\n");
		for(i = 0; i < q->nads; i++) {
			strcat(b,q->ads[i]);	
		}
			strcat(b,"END\n");
		}
	}
	if(q->options.misc) {
		if(q->nmisc > 0) {
			strcat(b,"MSC\n");
		for(i = 0; i < q->nmisc; i++) {
			strcat(b,q->misc[i]);	
		}
			strcat(b,"END\n");
		}
	}
	// sha1 hash
	// compress_buffer
}

// recurse

int fit_buffer(int results, int buffer_size, QueryData *q) {
	int result;
	if(results > q->nresults) {
		results = q->nresults;
	}
	char *buffer = calloc(buffer_size*2,1); // FIXME allocate somewhere else
	while(results > 0) {
		if((result = compress_results(q,buffer,results) > 0)) {
			return(result);
		} else {
			results--;
		}
	}
	if(result > 0) { return result; } else { return(results); }
}
