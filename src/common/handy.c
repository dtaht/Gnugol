
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <curl/curl.h>
#include "utf8.h"
#include "handy.h"

#define BUFFER_SIZE (64*1024)

// Convert html entities into an equivalent UTF-8 code

struct entitymap {
  char *entity;
  char *utf8;
};

// FIXME: Haven't got around to building a full lookup table yet.

static struct entitymap emap[] = { 
  { "quot", "\"" },
  { "middot", "·" },
  { "amp", "&" },
  { "lt", "<" },
  { "gt", ">" },
  { "#39","'" },
  { "nbsp","'" },
  { NULL, NULL },
};

int substitute_utf8(char *string, char *entity) {
  if(entity[0]=='&') entity = &entity[1];
  int len = strlen(entity);
  if(entity[len-1]==';') entity[len-1] = '\0'; 
  for(int i = 0; emap[i].entity != NULL; i++) { 
    if(strcmp(entity,emap[i].entity)==0) {
      strncpy(string,emap[i].utf8,strlen(emap[i].utf8));
    return(strlen(emap[i].utf8));
    }
  }
  return 0;
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
	    j += substitute_utf8(&string[j],entity);
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

long long dnslookup(char *name) {
}


/* Return the offset of the first newline in text or the length of
   text if there's no newline */

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
