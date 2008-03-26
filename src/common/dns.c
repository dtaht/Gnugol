// Lazy DNS

int getgnugolbyintuition();
int gethostbyintuition();

/* 
  Find available gnugol servers by "intuition".
  This includes: 
  reverse DNS lookup
  DNS record lookup
  Recursive DNS lookup - as in 
    gnugol.whatever.l.y.com 
    gnugol.l.y.com
    gnugol.y.com
    gnugol.com

  static configuration via conf file
  command line configuration
  gnugol broadcast 
  ???
  Not in this order.
*/

int getgnugolbyintuition() {
}

// This abuses the filesystem worse than early versions of git did
// It's terrible, as the largest file will be about 50 bytes in 
// length. Far better to adopt Berkley DB.

static FILE *search_dns(sha1 *sha1) {
  FILE *fp;
  char filename[PATH_LEN];
  char sha[40];
  // convert_to_sha_string(&sha,sha1);
  sprintf(filename,"%s" DIR_SEP "dns" DIR_SEP "%s[2]" DIR_SEP "%s[2]" DIR_SEP "%s[16]",
	   gnugol_cache_dir,&sha[0],&sha[2],&sha[4]);
  if(fp = fopen(filename,"rb")) { 
    return fp; 
  } else { 
    return (FILE *) NULL; 
  }
}

int dns_filesystem(sha1 *sha, QueryData *q) {
  char filename[PATH_LEN];
  FILE *fp;
  sprintf(filename,"%s" DIR_SEP "dns" DIR_SEP "%s[2]" DIR_SEP "%s[2]" DIR_SEP "%s[16]",
	   gnugol_cache_dir,&sha[0],&sha[2],&sha[4]);
  // FIXME, toy with the mtime, ctime to create expiry information
  // also creat directory structure if needed
  if(fp = fopen(filename,"wb")) { 
    fwrite(fp,q->result,q->size); 
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
  if((fp = search_filesystem(QueryData *q)) != NULL) {
    // FIXME, do consistency checks
  } 
}


// trust
