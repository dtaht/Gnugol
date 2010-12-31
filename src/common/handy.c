
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <locale.h>
#include <jansson.h>
#include <curl/curl.h>
#include "utf8.h"
#include "handy.h"

#define BUFFER_SIZE (64*1024)

// Convert html entities into an equivalent UTF-8 code

struct entitymap {
  const char *const entity;
  const int         utf8;
};

/*---------------------------------------------------------------------
; The following table is sorted as US-ASCII (the C locale) on the first
; column.  When adding new entries, make sure they're sorted properly.
;----------------------------------------------------------------------*/

static const struct entitymap emap[] = {
  { "AElig" 	,  198 },
  { "Aacute" 	,  193 },
  { "Acirc" 	,  194 },
  { "Agrave" 	,  192 },
  { "Alpha" 	,  913 },
  { "Aring" 	,  197 },
  { "Atilde" 	,  195 },
  { "Auml" 	,  196 },
  { "Beta" 	,  914 },
  { "Ccedil" 	,  199 },
  { "Chi" 	,  935 },
  { "Dagger" 	, 8225 },
  { "Delta" 	,  916 },
  { "ETH" 	,  208 },
  { "Eacute" 	,  201 },
  { "Ecirc" 	,  202 },
  { "Egrave" 	,  200 },
  { "Epsilon" 	,  917 },
  { "Eta" 	,  919 },
  { "Euml" 	,  203 },
  { "Gamma" 	,  915 },
  { "Iacute" 	,  205 },
  { "Icirc" 	,  206 },
  { "Igrave" 	,  204 },
  { "Iota" 	,  921 },
  { "Iuml" 	,  207 },
  { "Kappa" 	,  922 },
  { "Lambda" 	,  923 },
  { "Mu" 	,  924 },
  { "Ntilde" 	,  209 },
  { "Nu" 	,  925 },
  { "OElig" 	,  338 },
  { "Oacute" 	,  211 },
  { "Ocirc" 	,  212 },
  { "Ograve" 	,  210 },
  { "Omega" 	,  937 },
  { "Omicron" 	,  927 },
  { "Oslash" 	,  216 },
  { "Otilde" 	,  213 },
  { "Ouml" 	,  214 },
  { "Phi" 	,  934 },
  { "Pi" 	,  928 },
  { "Prime" 	, 8243 },
  { "Psi" 	,  936 },
  { "Rho" 	,  929 },
  { "Scaron" 	,  352 },
  { "Sigma" 	,  931 },
  { "THORN" 	,  222 },
  { "Tau" 	,  932 },
  { "Theta" 	,  920 },
  { "Uacute" 	,  218 },
  { "Ucirc" 	,  219 },
  { "Ugrave" 	,  217 },
  { "Upsilon" 	,  933 },
  { "Uuml" 	,  220 },
  { "Xi" 	,  926 },
  { "Yacute" 	,  221 },
  { "Yuml" 	,  376 },
  { "Zeta" 	,  918 },
  { "aacute" 	,  225 },
  { "acirc" 	,  226 },
  { "acute" 	,  180 },
  { "aelig" 	,  230 },
  { "agrave" 	,  224 },
  { "alefsym" 	, 8501 },
  { "alpha" 	,  945 },
  { "amp" 	,   38 },
  { "and" 	, 8743 },
  { "ang" 	, 8736 },
  { "aring" 	,  229 },
  { "asymp" 	, 8776 },
  { "atilde" 	,  227 },
  { "auml" 	,  228 },
  { "bdquo" 	, 8222 },
  { "beta" 	,  946 },
  { "brvbar" 	,  166 },
  { "bull" 	, 8226 },
  { "cap" 	, 8745 },
  { "ccedil" 	,  231 },
  { "cedil" 	,  184 },
  { "cent" 	,  162 },
  { "chi" 	,  967 },
  { "circ" 	,  710 },
  { "clubs" 	, 9827 },
  { "cong" 	, 8773 },
  { "copy" 	,  169 },
  { "crarr" 	, 8629 },
  { "cup" 	, 8746 },
  { "curren" 	,  164 },
  { "dArr" 	, 8659 },
  { "dagger" 	, 8224 },
  { "darr" 	, 8595 },
  { "deg" 	,  176 },
  { "delta" 	,  948 },
  { "diams" 	, 9830 },
  { "divide" 	,  247 },
  { "eacute" 	,  233 },
  { "ecirc" 	,  234 },
  { "egrave" 	,  232 },
  { "empty" 	, 8709 },
  { "emsp" 	, 8195 },
  { "ensp" 	, 8194 },
  { "epsilon" 	,  949 },
  { "equiv" 	, 8801 },
  { "eta" 	,  951 },
  { "eth" 	,  240 },
  { "euml" 	,  235 },
  { "euro" 	, 8364 },
  { "exist" 	, 8707 },
  { "fnof" 	,  402 },
  { "forall" 	, 8704 },
  { "frac12" 	,  189 },
  { "frac14" 	,  188 },
  { "frac34" 	,  190 },
  { "frasl" 	, 8260 },
  { "gamma" 	,  947 },
  { "ge" 	, 8805 },
  { "gt" 	,   62 },
  { "hArr" 	, 8660 },
  { "harr" 	, 8596 },
  { "hearts" 	, 9829 },
  { "hellip" 	, 8230 },
  { "iacute" 	,  237 },
  { "icirc" 	,  238 },
  { "iexcl" 	,  161 },
  { "igrave" 	,  236 },
  { "image" 	, 8465 },
  { "infin" 	, 8734 },
  { "int" 	, 8747 },
  { "iota" 	,  953 },
  { "iquest" 	,  191 },
  { "isin" 	, 8712 },
  { "iuml" 	,  239 },
  { "kappa" 	,  954 },
  { "lArr" 	, 8656 },
  { "lambda" 	,  955 },
  { "lang" 	, 9001 },
  { "laquo" 	,  171 },
  { "larr" 	, 8592 },
  { "lceil" 	, 8968 },
  { "ldquo" 	, 8220 },
  { "le" 	, 8804 },
  { "lfloor" 	, 8970 },
  { "lowast" 	, 8727 },
  { "loz" 	, 9674 },
  { "lrm" 	, 8206 },
  { "lsaquo" 	, 8249 },
  { "lsquo" 	, 8216 },
  { "lt" 	,   60 },
  { "macr" 	,  175 },
  { "mdash" 	, 8212 },
  { "micro" 	,  181 },
  { "middot" 	,  183 },
  { "minus" 	, 8722 },
  { "mu" 	,  956 },
  { "nabla" 	, 8711 },
  { "nbsp" 	,  160 },
  { "ndash" 	, 8211 },
  { "ne" 	, 8800 },
  { "ni" 	, 8715 },
  { "not" 	,  172 },
  { "notin" 	, 8713 },
  { "nsub" 	, 8836 },
  { "ntilde" 	,  241 },
  { "nu" 	,  957 },
  { "oacute" 	,  243 },
  { "ocirc" 	,  244 },
  { "oelig" 	,  339 },
  { "ograve" 	,  242 },
  { "oline" 	, 8254 },
  { "omega" 	,  969 },
  { "omicron" 	,  959 },
  { "oplus" 	, 8853 },
  { "or" 	, 8744 },
  { "ordf" 	,  170 },
  { "ordm" 	,  186 },
  { "oslash" 	,  248 },
  { "otilde" 	,  245 },
  { "otimes" 	, 8855 },
  { "ouml" 	,  246 },
  { "para" 	,  182 },
  { "part" 	, 8706 },
  { "permil" 	, 8240 },
  { "perp" 	, 8869 },
  { "phi" 	,  966 },
  { "pi" 	,  960 },
  { "piv" 	,  982 },
  { "plusmn" 	,  177 },
  { "pound" 	,  163 },
  { "prime" 	, 8242 },
  { "prod" 	, 8719 },
  { "prop" 	, 8733 },
  { "psi" 	,  968 },
  { "quot" 	,   34 },
  { "rArr" 	, 8658 },
  { "radic" 	, 8730 },
  { "rang" 	, 9002 },
  { "raquo" 	,  187 },
  { "rarr" 	, 8594 },
  { "rceil" 	, 8969 },
  { "rdquo" 	, 8221 },
  { "real" 	, 8476 },
  { "reg" 	,  174 },
  { "rfloor" 	, 8971 },
  { "rho" 	,  961 },
  { "rlm" 	, 8207 },
  { "rsaquo" 	, 8250 },
  { "rsquo" 	, 8217 },
  { "sbquo" 	, 8218 },
  { "scaron" 	,  353 },
  { "sdot" 	, 8901 },
  { "sect" 	,  167 },
  { "shy" 	,  173 },
  { "sigma" 	,  963 },
  { "sigmaf" 	,  962 },
  { "sim" 	, 8764 },
  { "spades" 	, 9824 },
  { "sub" 	, 8834 },
  { "sube" 	, 8838 },
  { "sum" 	, 8721 },
  { "sup" 	, 8835 },
  { "sup1" 	,  185 },
  { "sup2" 	,  178 },
  { "sup3" 	,  179 },
  { "supe" 	, 8839 },
  { "szlig" 	,  223 },
  { "tau" 	,  964 },
  { "there4" 	, 8756 },
  { "theta" 	,  952 },
  { "thetasym" 	,  977 },
  { "thinsp" 	, 8201 },
  { "thorn" 	,  254 },
  { "tilde" 	,  732 },
  { "times" 	,  215 },
  { "trade" 	, 8482 },
  { "uArr" 	, 8657 },
  { "uacute" 	,  250 },
  { "uarr" 	, 8593 },
  { "ucirc" 	,  251 },
  { "ugrave" 	,  249 },
  { "uml" 	,  168 },
  { "upsih" 	,  978 },
  { "upsilon" 	,  965 },
  { "uuml" 	,  252 },
  { "weierp" 	, 8472 },
  { "xi" 	,  958 },
  { "yacute" 	,  253 },
  { "yen" 	,  165 },
  { "yuml" 	,  255 },
  { "zeta" 	,  950 },
  { "zwj" 	, 8205 },
  { "zwnj" 	, 8204 }
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
  assert(string  != NULL);
  assert(entity  != NULL);
  assert(*entity == '&');
  if (lenstr < 5)	/* not enough space for worst case scenario */
    return 0;

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
  char                    buf[5];
  u_int32_t               value;

  entity++;
  len = strlen(entity);
  if(entity[len-1]==';') entity[len-1] = '\0';

  /*----------------------------------------------------------------
  ; if the entity is in the form of &#nn; then nn is the Unicode code point
  ; for the character (it's in numeric form).  If it's in that form, convert
  ; to a proper UTF-8 charracter, otherwise, lookup the symbolic form, get
  ; it's codepoint and convert.  If the character isn't found, return '?'
  ;-------------------------------------------------------------------------*/

  if (entity[0] == '#')
    value = strtoul(&entity[1],NULL,10);
  else
  {
    e = bsearch(entity,emap,ENTITIES,sizeof(struct entitymap),entity_cmp);
    if (e != NULL)
      value = e->utf8;
    else
      value = '?';
  }

  size = u8_wc_toutf8(buf,value);
  assert(size <= 4);
  memcpy(string,buf,size);
  string[size] = '\0';

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
