/* Fit the results into a buffer of size X */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <limits.h>
#include "query.h"
#include "gnugol_protocol.h"

#ifdef WINDOWS
#define DIR_SEP "\""
#else
#define DIR_SEP "/"
#endif

static char *gnugol_cache_dir = "/var/cache/gnugol";

int build_query(QueryData *q) {
  char *query = q->query;
  sprintf(query,"GET GNGL/%g ", GNUGOL_PROTOCOL_VERSION);

  if(q->options.urls) strcat(query, "LNK ");
  if(q->options.titles) strcat(query, "TLE ");
  if(q->options.snippets) strcat(query, "SNP ");
  if(q->options.ads) strcat(query, "ADS ");
  if(q->options.misc) strcat(query, "MSC ");
  if(q->options.prime) strcat(query, "PRM ");
  if(q->options.position != 0) sprintf(query, "%sPOS:%d ", query, q->options.position);
  if(q->options.nresults != 10) sprintf(query, "%sRES:%d ", query, q->options.nresults);
 
  strcat(query,"\n");
  if(q->keywords[0] == '\0') { 
    if(q->options.prime) { 
      fprintf(stderr, "Null query data, just priming the connnection\n"); 
      strcat(query,"\n");
    } else {
      fprintf(stderr, "Null query data, not sending!\n"); return(-1); 
    }
  } else {
    strcat(query,q->keywords);
    if(q->options.debug) { 
    fprintf(stderr,"Keywords:%s \n", q->keywords);
    }
}
  strcat(query,"\nEND\n");
  return(0);
}

// Abuse the filesystem like early versions of git did
// It's even worse than early git here, as the largest file will be
// 1280 bytes

struct sha1_t {
  char s[20];
};

//typedef sha1_t sha1;

static FILE *search_filesystem(struct sha1_t *sha1) {
  FILE *fp;
  char filename[PATH_MAX];
  char sha[40];
  // convert_to_sha_string(&sha,sha1);
  // FIXME for windows
  sprintf(filename,"%s" DIR_SEP "queries" DIR_SEP "%s[2]" DIR_SEP "%s[2]" DIR_SEP "%s[16]",
	   gnugol_cache_dir,&sha[0],&sha[2],&sha[4]);
  if(fp = fopen(filename,"rb")) { 
    return fp; 
  } else { 
    return (FILE *) NULL; 
  }
}

int update_filesystem(char *sha, QueryData *q) {
  char filename[PATH_MAX];
  FILE *fp;
  sprintf(filename,"%s" DIR_SEP "queries" DIR_SEP "%s[2]" DIR_SEP "%s[2]" DIR_SEP "%s[16]",
	   gnugol_cache_dir,&sha[0],&sha[2],&sha[4]);
  // FIXME, toy with the mtime, ctime to create expiry information
  // also creat directory structure if needed
  if(fp = fopen(filename,"wb")) { 
    // FIXME!! WTF WAS RESULT?    fwrite(fp,q->result,q->size); 
    fclose(fp);
    return 0; 
  } else { 
    return -1; 
  }
}

// Not implemented yet
int lookaside_cache(QueryData *q) {
  FILE *fp;
  return(1);
  if((fp = search_filesystem(q)) != NULL) {
    // FIXME, do consistency checks
  } 
}

int uncompress_results(QueryData *q) {
}

int
compress_results(QueryData *q, char *packet, int size) { // size will usually equal 1240
	int i;
	int nbytes = 0;
	// For now, don't compress
	for(i = 0; i < q->nresults-1 && nbytes < MAX_MTU - 40; i++) { 
	  nbytes += q->options.urls 	? q->nlinkbytes[i]    : 0; 
	  nbytes += q->options.snippets ? q->nsnippetbytes[i] : 0;
	  nbytes += q->options.titles 	? q->ntitlebytes[i]   : 0;
	  nbytes += q->options.ads 	? q->nadbytes[i]      : 0;
	  nbytes += q->options.misc 	? q->nmiscbytes[i]    : 0;
	}
	if(nbytes == 0) { fprintf(stderr,"No data\n"); return 0; }
	if(nbytes > 4*size) { 
		// odds are really good that we can't compress that many results, don't try
		return(-1); 
	}

	int resulttypes = q->options.urls + q->options.snippets + q->options.titles +
	  q->options.ads + q->options.misc;
	char *b = (char *) calloc (nbytes + 8*resulttypes,1);

	if(q->options.urls && q->nurls > 0) {
	  strcat(b,"LNK\n");
	  for(i = 0; i < q->nurls + 1; i++) {
	    strcat(b,q->links[i]);	
	  }
	  strcat(b,"END\n");
	}
	if(q->options.snippets && q->nsnippets > 0) {
	  strcat(b,"SNP\n"); // FIXME
	  for(i = 0; i < q->nsnippets + 1; i++) {
	    strcat(b,q->snippets[i]);	
	  }
	  strcat(b,"END\n");
	}
	if(q->options.titles && q->ntitles > 0) {
	  strcat(b,"TLE\n");
	  for(i = 0; i < q->ntitles + 1; i++) {
	    strcat(b,q->titles[i]);	
	  }
	  strcat(b,"END\n");
	}
	if(q->options.ads && q->nads > 0) {
	  strcat(b,"ADS\n");
	  for(i = 0; i < q->nads + 1; i++) {
	    strcat(b,q->ads[i]);	
	  }
	  strcat(b,"END\n");
	}
	if(q->options.misc && q->nmisc > 0) {
	  strcat(b,"MSC\n");
	  for(i = 0; i < q->nmisc + 1; i++) {
	    strcat(b,q->misc[i]);	
	  }
	  strcat(b,"END\n");
	}
	// sha1 hash
	// compress_buffer
}

process_network_buffer (char *packet) {
  
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
