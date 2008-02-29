/* Fit the results into a buffer of size X */
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include "query.h"

int
compress_results(QueryData *q, char *packet, int size) { // size will usually equal 1240
	int i;
	int nsize = 0;
	nsize += q->options.urls 	? 0 : q->nurls; // BAD!! FIXME, this is the number of urls, not the size
	nsize += q->options.snippets 	? 0 : q->nsnippets;
	nsize += q->options.titles 	? 0 : q->ntitles;
	nsize += q->options.ads 	? 0 : q->nads;
	nsize += q->options.misc 	? 0 : q->nmisc;

	if(nsize > 4*size) { 
		return(-1); // odds are really good that we can't compress that many results, don't try
	}
	char *b = (char *) calloc (nsize + 40,1); // at most we have 10 4 character delimters

	// yea, yea, strcat's inefficient
	// FIXME - retain the protocol here

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
