
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <locale.h>
#include <jansson.h>
#include <curl/curl.h>
#include "utf8.h"
#include "handy.h"

#ifndef __GNUC__
#  define __attribute__(x)
#endif

#define BUFFER_SIZE (64*1024)

// Convert html entities into an equivalent UTF-8 code

struct entitymap {
  const char *const entity;
  const char *const utf8;
};

/*---------------------------------------------------------------------
; The following table is sorted as US-ASCII (the C locale) on the first
; column.  When adding new entries, make sure they're sorted properly.
;----------------------------------------------------------------------*/

static const struct entitymap emap[] = { 
  { "AElig" 	, "\303\206"	 } ,
  { "Aacute" 	, "\303\201"	 } ,
  { "Acirc" 	, "\303\202"	 } ,
  { "Agrave" 	, "\303\200"	 } ,
  { "Alpha" 	, "\316\221"	 } ,
  { "Aring" 	, "\303\205"	 } ,
  { "Atilde" 	, "\303\203"	 } ,
  { "Auml" 	, "\303\204"	 } ,
  { "Beta" 	, "\316\222"	 } ,
  { "Ccedil" 	, "\303\207"	 } ,
  { "Chi" 	, "\316\247"	 } ,
  { "Dagger" 	, "\342\200\241" } ,
  { "Delta" 	, "\316\224"	 } ,
  { "ETH" 	, "\303\220"	 } ,
  { "Eacute" 	, "\303\211"	 } ,
  { "Ecirc" 	, "\303\212"	 } ,
  { "Egrave" 	, "\303\210"	 } ,
  { "Epsilon" 	, "\316\225"	 } ,
  { "Eta" 	, "\316\227"	 } ,
  { "Euml" 	, "\303\213"	 } ,
  { "Gamma" 	, "\316\223"	 } ,
  { "Iacute" 	, "\303\215"	 } ,
  { "Icirc" 	, "\303\216"	 } ,
  { "Igrave" 	, "\303\214"	 } ,
  { "Iota" 	, "\316\231"	 } ,
  { "Iuml" 	, "\303\217"	 } ,
  { "Kappa" 	, "\316\232"	 } ,
  { "Lambda" 	, "\316\233"	 } ,
  { "Mu" 	, "\316\234"	 } ,
  { "Ntilde" 	, "\303\221"	 } ,
  { "Nu" 	, "\316\235"	 } ,
  { "OElig" 	, "\305\222"	 } ,
  { "Oacute" 	, "\303\223"	 } ,
  { "Ocirc" 	, "\303\224"	 } ,
  { "Ograve" 	, "\303\222"	 } ,
  { "Omega" 	, "\316\251"	 } ,
  { "Omicron" 	, "\316\237"	 } ,
  { "Oslash" 	, "\303\230"	 } ,
  { "Otilde" 	, "\303\225"	 } ,
  { "Ouml" 	, "\303\226"	 } ,
  { "Phi" 	, "\316\246"	 } ,
  { "Pi" 	, "\316\240"	 } ,
  { "Prime" 	, "\342\200\263" } ,
  { "Psi" 	, "\316\250"	 } ,
  { "Rho" 	, "\316\241"	 } ,
  { "Scaron" 	, "\305\240"	 } ,
  { "Sigma" 	, "\316\243"	 } ,
  { "THORN" 	, "\303\236"	 } ,
  { "Tau" 	, "\316\244"	 } ,
  { "Theta" 	, "\316\230"	 } ,
  { "Uacute" 	, "\303\232"	 } ,
  { "Ucirc" 	, "\303\233"	 } ,
  { "Ugrave" 	, "\303\231"	 } ,
  { "Upsilon" 	, "\316\245"	 } ,
  { "Uuml" 	, "\303\234"	 } ,
  { "Xi" 	, "\316\236"	 } ,
  { "Yacute" 	, "\303\235"	 } ,
  { "Yuml" 	, "\305\270"	 } ,
  { "Zeta" 	, "\316\226"	 } ,
  { "aacute" 	, "\303\241"	 } ,
  { "acirc" 	, "\303\242"	 } ,
  { "acute" 	, "\302\264"	 } ,
  { "aelig" 	, "\303\246"	 } ,
  { "agrave" 	, "\303\240"	 } ,
  { "alefsym" 	, "\342\204\265" } ,
  { "alpha" 	, "\316\261"	 } ,
  { "amp" 	, "\046"	 } ,
  { "and" 	, "\342\210\247" } ,
  { "ang" 	, "\342\210\240" } ,
  { "aring" 	, "\303\245"	 } ,
  { "asymp" 	, "\342\211\210" } ,
  { "atilde" 	, "\303\243"	 } ,
  { "auml" 	, "\303\244"	 } ,
  { "bdquo" 	, "\342\200\236" } ,
  { "beta" 	, "\316\262"	 } ,
  { "brvbar" 	, "\302\246"	 } ,
  { "bull" 	, "\342\200\242" } ,
  { "cap" 	, "\342\210\251" } ,
  { "ccedil" 	, "\303\247"	 } ,
  { "cedil" 	, "\302\270"	 } ,
  { "cent" 	, "\302\242"	 } ,
  { "chi" 	, "\317\207"	 } ,
  { "circ" 	, "\313\206"	 } ,
  { "clubs" 	, "\342\231\243" } ,
  { "cong" 	, "\342\211\205" } ,
  { "copy" 	, "\302\251"	 } ,
  { "crarr" 	, "\342\206\265" } ,
  { "cup" 	, "\342\210\252" } ,
  { "curren" 	, "\302\244"	 } ,
  { "dArr" 	, "\342\207\223" } ,
  { "dagger" 	, "\342\200\240" } ,
  { "darr" 	, "\342\206\223" } ,
  { "deg" 	, "\302\260"	 } ,
  { "delta" 	, "\316\264"	 } ,
  { "diams" 	, "\342\231\246" } ,
  { "divide" 	, "\303\267"	 } ,
  { "eacute" 	, "\303\251"	 } ,
  { "ecirc" 	, "\303\252"	 } ,
  { "egrave" 	, "\303\250"	 } ,
  { "empty" 	, "\342\210\205" } ,
  { "emsp" 	, "\342\200\203" } ,
  { "ensp" 	, "\342\200\202" } ,
  { "epsilon" 	, "\316\265"	 } ,
  { "equiv" 	, "\342\211\241" } ,
  { "eta" 	, "\316\267"	 } ,
  { "eth" 	, "\303\260"	 } ,
  { "euml" 	, "\303\253"	 } ,
  { "euro" 	, "\342\202\254" } ,
  { "exist" 	, "\342\210\203" } ,
  { "fnof" 	, "\306\222"	 } ,
  { "forall" 	, "\342\210\200" } ,
  { "frac12" 	, "\302\275"	 } ,
  { "frac14" 	, "\302\274"	 } ,
  { "frac34" 	, "\302\276"	 } ,
  { "frasl" 	, "\342\201\204" } ,
  { "gamma" 	, "\316\263"	 } ,
  { "ge" 	, "\342\211\245" } ,
  { "gt" 	, "\076"	 } ,
  { "hArr" 	, "\342\207\224" } ,
  { "harr" 	, "\342\206\224" } ,
  { "hearts" 	, "\342\231\245" } ,
  { "hellip" 	, "\342\200\246" } ,
  { "iacute" 	, "\303\255"	 } ,
  { "icirc" 	, "\303\256"	 } ,
  { "iexcl" 	, "\302\241"	 } ,
  { "igrave" 	, "\303\254"	 } ,
  { "image" 	, "\342\204\221" } ,
  { "infin" 	, "\342\210\236" } ,
  { "int" 	, "\342\210\253" } ,
  { "iota" 	, "\316\271"	 } ,
  { "iquest" 	, "\302\277"	 } ,
  { "isin" 	, "\342\210\210" } ,
  { "iuml" 	, "\303\257"	 } ,
  { "kappa" 	, "\316\272"	 } ,
  { "lArr" 	, "\342\207\220" } ,
  { "lambda" 	, "\316\273"	 } ,
  { "lang" 	, "\342\214\251" } ,
  { "laquo" 	, "\302\253"	 } ,
  { "larr" 	, "\342\206\220" } ,
  { "lceil" 	, "\342\214\210" } ,
  { "ldquo" 	, "\342\200\234" } ,
  { "le" 	, "\342\211\244" } ,
  { "lfloor" 	, "\342\214\212" } ,
  { "lowast" 	, "\342\210\227" } ,
  { "loz" 	, "\342\227\212" } ,
  { "lrm" 	, "\342\200\216" } ,
  { "lsaquo" 	, "\342\200\271" } ,
  { "lsquo" 	, "\342\200\230" } ,
  { "lt" 	, "\074"	 } ,
  { "macr" 	, "\302\257"	 } ,
  { "mdash" 	, "\342\200\224" } ,
  { "micro" 	, "\302\265"	 } ,
  { "middot" 	, "\302\267"	 } ,
  { "minus" 	, "\342\210\222" } ,
  { "mu" 	, "\316\274"	 } ,
  { "nabla" 	, "\342\210\207" } ,
  { "nbsp" 	, "\302\240"	 } ,
  { "ndash" 	, "\342\200\223" } ,
  { "ne" 	, "\342\211\240" } ,
  { "ni" 	, "\342\210\213" } ,
  { "not" 	, "\302\254"	 } ,
  { "notin" 	, "\342\210\211" } ,
  { "nsub" 	, "\342\212\204" } ,
  { "ntilde" 	, "\303\261"	 } ,
  { "nu" 	, "\316\275"	 } ,
  { "oacute" 	, "\303\263"	 } ,
  { "ocirc" 	, "\303\264"	 } ,
  { "oelig" 	, "\305\223"	 } ,
  { "ograve" 	, "\303\262"	 } ,
  { "oline" 	, "\342\200\276" } ,
  { "omega" 	, "\317\211"	 } ,
  { "omicron" 	, "\316\277"	 } ,
  { "oplus" 	, "\342\212\225" } ,
  { "or" 	, "\342\210\250" } ,
  { "ordf" 	, "\302\252"	 } ,
  { "ordm" 	, "\302\272"	 } ,
  { "oslash" 	, "\303\270"	 } ,
  { "otilde" 	, "\303\265"	 } ,
  { "otimes" 	, "\342\212\227" } ,
  { "ouml" 	, "\303\266"	 } ,
  { "para" 	, "\302\266"	 } ,
  { "part" 	, "\342\210\202" } ,
  { "permil" 	, "\342\200\260" } ,
  { "perp" 	, "\342\212\245" } ,
  { "phi" 	, "\317\206"	 } ,
  { "pi" 	, "\317\200"	 } ,
  { "piv" 	, "\317\226"	 } ,
  { "plusmn" 	, "\302\261"	 } ,
  { "pound" 	, "\302\243"	 } ,
  { "prime" 	, "\342\200\262" } ,
  { "prod" 	, "\342\210\217" } ,
  { "prop" 	, "\342\210\235" } ,
  { "psi" 	, "\317\210"	 } ,
  { "quot" 	, "\042"	 } ,
  { "rArr" 	, "\342\207\222" } ,
  { "radic" 	, "\342\210\232" } ,
  { "rang" 	, "\342\214\252" } ,
  { "raquo" 	, "\302\273"	 } ,
  { "rarr" 	, "\342\206\222" } ,
  { "rceil" 	, "\342\214\211" } ,
  { "rdquo" 	, "\342\200\235" } ,
  { "real" 	, "\342\204\234" } ,
  { "reg" 	, "\302\256"	 } ,
  { "rfloor" 	, "\342\214\213" } ,
  { "rho" 	, "\317\201"	 } ,
  { "rlm" 	, "\342\200\217" } ,
  { "rsaquo" 	, "\342\200\272" } ,
  { "rsquo" 	, "\342\200\231" } ,
  { "sbquo" 	, "\342\200\232" } ,
  { "scaron" 	, "\305\241"	 } ,
  { "sdot" 	, "\342\213\205" } ,
  { "sect" 	, "\302\247"	 } ,
  { "shy" 	, "\302\255"	 } ,
  { "sigma" 	, "\317\203"	 } ,
  { "sigmaf" 	, "\317\202"	 } ,
  { "sim" 	, "\342\210\274" } ,
  { "spades" 	, "\342\231\240" } ,
  { "sub" 	, "\342\212\202" } ,
  { "sube" 	, "\342\212\206" } ,
  { "sum" 	, "\342\210\221" } ,
  { "sup" 	, "\342\212\203" } ,
  { "sup1" 	, "\302\271"	 } ,
  { "sup2" 	, "\302\262"	 } ,
  { "sup3" 	, "\302\263"	 } ,
  { "supe" 	, "\342\212\207" } ,
  { "szlig" 	, "\303\237"	 } ,
  { "tau" 	, "\317\204"	 } ,
  { "there4" 	, "\342\210\264" } ,
  { "theta" 	, "\316\270"	 } ,
  { "thetasym" 	, "\317\221"	 } ,
  { "thinsp" 	, "\342\200\211" } ,
  { "thorn" 	, "\303\276"	 } ,
  { "tilde" 	, "\313\234"	 } ,
  { "times" 	, "\303\227"	 } ,
  { "trade" 	, "\342\204\242" } ,
  { "uArr" 	, "\342\207\221" } ,
  { "uacute" 	, "\303\272"	 } ,
  { "uarr" 	, "\342\206\221" } ,
  { "ucirc" 	, "\303\273"	 } ,
  { "ugrave" 	, "\303\271"	 } ,
  { "uml" 	, "\302\250"	 } ,
  { "upsih" 	, "\317\222"	 } ,
  { "upsilon" 	, "\317\205"	 } ,
  { "uuml" 	, "\303\274"	 } ,
  { "weierp" 	, "\342\204\230" } ,
  { "xi" 	, "\316\276"	 } ,
  { "yacute" 	, "\303\275"	 } ,
  { "yen" 	, "\302\245"	 } ,
  { "yuml" 	, "\303\277"	 } ,
  { "zeta" 	, "\316\266"	 } ,
  { "zwj" 	, "\342\200\215" } ,
  { "zwnj" 	, "\342\200\214" } ,
};

#define ENTITIES (sizeof(emap) / sizeof(struct entitymap))

static int entity_cmp(const void *needle,const void *haystack)
{
  const char             *key = needle;
  const struct entitymap *hay = haystack;
  
  return strcmp(key,hay->entity);
}

size_t substitute_utf8(char *string,size_t lenstr,char *entity)
{
  /*-------------------------------------------------------------------------
  ; The above table is sorted using the C collate sequence (US-ASCII) so we
  ; need to make sure our locale is set so a subsequent search will return
  ; the appropriate value.  But since I don't want to make any assumptions
  ; as to the locale elsewhere in the code, we have the current locale, set
  ; it, then restore it upon return.
  ;-------------------------------------------------------------------------*/
  
  char *collate = setlocale(LC_COLLATE,NULL);
  if (collate == NULL)
    collate = "";
  
  char savecollate[strlen(collate) + 1];
  memcpy(savecollate,collate,strlen(collate) + 1);
  
  setlocale(LC_COLLATE,"C");
  
  const struct entitymap *e;
  size_t                  len;
  size_t                  size;
  size_t                  utfs;
  
  if(entity[0]=='&') entity = &entity[1];
  len = strlen(entity);
  if(entity[len-1]==';') entity[len-1] = '\0'; 
  
  e = bsearch(entity,emap,ENTITIES,sizeof(struct entitymap),entity_cmp);
  
  if (e != NULL)
  {
    utfs = strlen(e->utf8);
    size = (lenstr < utfs) ? lenstr : utfs;
    strncpy(string,e->utf8,size);
  }
  else
    size = 0;

  setlocale(LC_COLLATE,savecollate);  
  return size;
}

// Strip out newlines from string, too.
static int strip_newlines(char *string, int len) {
  int j = 0;
  for (int i = 0; i < len; i++) if(string[i] != '\n' &&string[i] != '\r') string[j++] = string[i]; 
  string[j] = '\0';
  return(j);
}

int format_term(char * string, int len) {
  int columns = 80;
  char *cols = getenv("COLUMNS");
  if(cols) columns = atoi(cols);
  if(columns < 20) columns = 20;
  columns = columns-2; // WRAP at columns - 2;
  int newlen = strip_newlines(string, strlen(string));
  // Handling utf-8 is tricky, we can't work by bytes
  int newstrlen = newlen+u8_strlen(string)/columns+1;
  if(newstrlen < len) 
  {
    char newstr[newstrlen];
    do {
    } while ( 0 );
    memcpy(string, newstr, strlen(newstr));  
  }
  return(newstrlen);
}

// Instead of a regex, this is a small state machine for what I want
// We eliminate multiple spaces, ellipses and anything that's inside 
// html. We also convert html entities into the equivalent utf-8.

int strip_html(int len, char *htmlstr) {
  int isspace = 0;
  int inhtml = 0;
  int inperiod = 0;
  int inentity = 0;
  int entitypos = 0;

  char entity[12];
  int j = 0;
  char string[len+1]; // yea, c99 lets me do this

  //  fprintf(stderr,"%s\n", htmlstr);
  for(int i = 0; i < len && htmlstr[i] != '\0'; i++) {
    switch(htmlstr[i]) {
    case '<': inhtml = 1; continue; break; // Start ignoring everything - malformed html >> is a problem.
    case '>': inhtml = 0; continue; break; // Stop ignoring everything
    default: break;
    }

    /* if(!inhtml) { */
    /*   printf("%c",htmlstr[i]); */
    /*   continue;  */
    /* } */

    // State machine from hell

    if(!inhtml) {
      switch(htmlstr[i]) {
      case '&': 
	  if(isspace) isspace = 0;
	  if(inperiod) inperiod = 0;
	  inentity = 1; 
	  entitypos = i; 
	  break; 
      
      case ';': 
	if(!(isspace|inperiod)) {
	  if(inentity) { 
	    strncpy(entity,&htmlstr[entitypos],i-entitypos);
	    entity[i-entitypos] = '\0';
	    /* I think len - j is the correct length to pass in */
	    j += substitute_utf8(&string[j],len - j,entity);
	    inentity = 0; 
	  } else {
	    string[j++] = ';';
	  }
	} else {
	  if(isspace) { 
	    string[j++] = ';'; 
	    isspace = 0; 
	  } else {
	    if(inperiod) 
	      { 
		string[j++] = ';'; 
		inperiod = 0; 
	      } 
	  }
	}
	
	break; 
      
      case ' ': 
      case '\n': 
      case '\r':
	if(isspace == 0) { 
	  isspace = 1; 
	  if(j > 0 && string[j-1] != ' ') 
	    string[j++] = ' '; 
	} 
	if(inperiod) inperiod = 0;
	if(inentity) inentity = 0;
	break;

      case '.': 
	if((inperiod == 1) && !(isspace || inentity)) 
	  { 
	    j--; 
	    inperiod++; // if we have more than one period strip it out 
	    break; 
	  }
	if ((inperiod == 0) && !(isspace || inentity)) 
	  { 
	    string[j++] = '.';  
	    inperiod = 1; 
	    break; 
	  }
	if((inperiod == 1) && (isspace)) 
	  { 
	    j--; 
	    inperiod++; // if we have more than one period strip it out 
	    isspace = 0;
	    break; 
	  }
	if((inperiod == 0) && (isspace)) 
	  { 
	    string[j++] = '.';  
	    inperiod = 1; 
	    isspace = 0;
	    break; 
	  }
      
	break;
      default: 
	isspace = 0;
	inperiod = 0;
	if(!inentity) string[j++] = htmlstr[i];
      }
    }
  }
  strncpy(htmlstr,string,j);
  htmlstr[j] = '\0';
  return j;
}

long long dnslookup(char *name __attribute__((unused))) {
  return 0;
}


/* Return the offset of the first newline in text or the length of
   text if there's no newline */

static int newline_offset(const char *) __attribute__ ((unused));
static int newline_offset(const char *text)
{
    const char *newline = strchr(text, '\n');
    if(!newline)
        return strlen(text);
    else
        return (int)(newline - text);
}

struct write_result
{
    char *data;
    int pos;
};

static size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream)
{
    struct write_result *result = (struct write_result *)stream;

    if(result->pos + size * nmemb >= BUFFER_SIZE - 1)
    {
        fprintf(stderr, "error: too small buffer\n");
        return 0;
    }

    memcpy(result->data + result->pos, ptr, size * nmemb);
    result->pos += size * nmemb;

    return size * nmemb;
}

char *jsonrequest(const char *url)
{
    CURL *curl;
    CURLcode status;
    char *data;
    long code;

    curl = curl_easy_init();
    data = malloc(BUFFER_SIZE);
    if(!curl || !data)
        return NULL;

    struct write_result write_result = {
        .data = data,
        .pos = 0
    };

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_ENCODING, "gzip");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_result);

    status = curl_easy_perform(curl);
    if(status != 0)
    {
        fprintf(stderr, "error: unable to request data from %s:\n", url);
        fprintf(stderr, "%s\n", curl_easy_strerror(status));
        return NULL;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if(code != 200)
    {
        fprintf(stderr, "error: server responded with code %ld\n", code);
        return NULL;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* zero-terminate the result */
    data[write_result.pos] = '\0';

    return data;
}
