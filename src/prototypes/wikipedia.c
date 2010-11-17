#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <curl/curl.h>


/* 1. $url = sprintf('http://en.wikipedia.org/w/api.php?action=query&titles=%s&prop=info&format=json', urlencode($search));  

   Once you have the page, you can get the text by

   time curl 'http://en.wikipedia.org/w/api.php?action=parse&page=IPv4&prop=text&format=json'

   2. $ch=curl_init();  
   3. curl_setopt($ch, CURLOPT_URL, $url);  
   4. curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);  
   5. curl_setopt($ch, CURLOPT_USERAGENT, 'your website address or app name');   
   6. $res = curl_exec($ch);  
   7. curl_close($ch);  
   8. require_once 'Zend/Json.php';  
   9. $val = Zend_Json::decode($res);  
 */

#define TEMPLATE  "http://ajax.googleapis.com/ajax/services/search/web?v=1.0"
#define BUFFER_SIZE  (64 * 1024)  /* 64 KB */
#define URL_SIZE     (2048)

// <a href=url>title</a> content
#define FORMATHTML 0

// [[Title|url]] content
#define FORMATWIKI 1

// * [[Title][url]] content
#define FORMATORG  2

// url title content
#define FORMATTERM 3

// Tiny web page of results
#define FORMATELINKS 4

#define FORMATDEFAULT 2

#define PATH_MAX 1024

/* See options at 
   http://code.google.com/apis/ajaxsearch/documentation/reference.html#_intro_fonje 
   including:
    rsz= large | small  (8 vs 4)
    start= <0-indexed offset>
    hl= <language of searcher>
    lr= <language of results>
    safe= active | moderate | off

*/

static int setup(char *string) {
  char path[PATH_MAX];
  char key[256];
  int fd;
  int size = 0;
  snprintf(path,PATH_MAX,"%s/%s",getenv("HOME"), ".googlekey");
  if(fd = open(path,O_RDONLY)) {
    size = read(fd, key, 256);
    while(size > 0 && (key[size-1] == ' ' || key[size-1] == '\n')) size--;
    key[size] = '\0';
  }
  // Wow. I did something here the compilere no like.
  if(size > 0) { 
    snprintf(string,URL_SIZE-1,"%s&key=%s&rsz=large",TEMPLATE,key); 
  } else {
    snprintf(string,URL_SIZE-1,"%s",TEMPLATE);
  }
  return size;
}

// Convert html entities into an equivalent UTF-8 code

struct entitymap {
  char *entity;
  char *utf8;
};

// Haven't got around to building a full lookup table yet.

static struct entitymap emap[] = { 
  { "quot", "\"" },
  { "middot", "·" },
  { "amp", "&" },
  { "lt", "<" },
  { "gt", ">" },
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

// this cries out for a regex
// ' ' <= <b>...</b> 
// <b>Whatever</b> *whatever*

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
	if(isspace == 0) { 
	  isspace = 1; 
	  if(j > 0 && string[j-1] != ' ') string[j++] = ' '; 
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

int usage(char *msg) {
  printf("%s\n",msg);
  exit(-1);
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

static char *request(const char *url)
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

// Might want a different call here for utf-8

#define jsv(a) json_string_value(a)

// Mapping one language to another requires 
// lots of repetetive code and error checking
// Macros may be ugly, but they do (in this case)
// ensure that all errors are captured.

// These macros assume that the first object is named root

#define GETOBJ(obj,newobj) do { newobj = json_object_get(obj, #newobj); \
    if(!newobj) { \
        fprintf(stderr, "error: " #newobj " is not an object\n"); \
	json_decref(root); \
        return 1;    } } while(1==0)

#define GETARRAY(obj,newobj) do { newobj = json_object_get(obj, #newobj); \
    if(!json_is_array(newobj)) {				       \
        fprintf(stderr, "error: " #newobj " is not an array\n"); \
	json_decref(root); \
        return 1;    } } while(1==0)

#define GETARRAYIDX(obj,newobj,idx) do { newobj = json_array_get(obj, idx); \
    if(!json_is_object(newobj)) {				       \
      fprintf(stderr, "error: " #newobj " is not an object\n", idx + 1);       \
	json_decref(root); \
        return 1;    } } while(1==0)

#define GETSTRING(obj,newobj) do { newobj = json_object_get(obj, #newobj); \
    if(!json_is_string(newobj)) {				       \
        fprintf(stderr, "error: " #newobj " is not a string\n"); \
	json_decref(root); \
        return 1;    } } while(1==0)

// for markdown get rid of <b>...</b>
// turn <b>whatever</b> into *whatever*
// turn quotes back into quotes and other utf-8 stuff
// FIXME Error outs cause a memory leak from "root"

int getresult(char *urltxt, int format) {
    unsigned int i;
    char *text;
    char url[URL_SIZE];
    json_t *root,*responseData, *results;
    json_error_t error;

    text = request(urltxt);
    if(!text)
        return 1;

    root = json_loads(text, &error);
    free(text);

    if(!root)
    {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return 1;
    }
    
    GETOBJ(root,responseData);
    GETARRAY(responseData,results);  

    switch(format) {
    case FORMATELINKS: 
      fprintf(stdout, "<html><head><title>Search for: ");
      fprintf(stdout, ""); // FIXME keywords
      fprintf(stdout, "</title></head><body>");
      break;
    case FORMATORG: 
      fprintf(stdout, "* Search: <keywords>"); // FIXME keywords
    default: break;
    }

    for(i = 0; i < json_array_size(results); i++)
    {
      json_t *result, *url, *titleNoFormatting, *content;
      const char *message_text;

      GETARRAYIDX(results,result,i);
      GETSTRING(result,url);
      GETSTRING(result,titleNoFormatting);
      GETSTRING(result,content);

      switch (format) {
	case FORMATWIKI: printf("[[%s|%s]] %s  \n",
				jsv(titleNoFormatting), 
				jsv(url), 
				jsv(content));  break;
	case FORMATORG:  
	  { 
	    char tempstr[2048]; 
	    strcpy(tempstr,jsv(titleNoFormatting));
	    strip_html(2048,tempstr);
	    printf("\n** [[%s][%s]]\n", jsv(url), tempstr);
	    strcpy(tempstr,jsv(content));
	    strip_html(2048,tempstr);
	    printf("   %s", tempstr); 
	  }
	    break;
	case FORMATTERM: 
	  { 
	    char tempstr[2048]; 
	    strcpy(tempstr,jsv(content));
	    strip_html(2048,tempstr);
	    printf("%s %s %s\n", jsv(url), 
		   jsv(titleNoFormatting), 
		   tempstr); 
	  }
	  break;

	case FORMATELINKS: printf("<p><a href=\"%s\">%s</a> %s</p>", 
				  jsv(url), 
				  jsv(titleNoFormatting), 
				  jsv(content)); break;

	default: printf("<a href=\"%s\">%s</a> %s\n", 
			jsv(url), jsv(titleNoFormatting), jsv(content)); 
	}
    } 

    if(format == FORMATELINKS) {
      fprintf(stdout, "</body></html>");
    }

    if(format == FORMATORG) {
      fprintf(stdout, "\n");
    }

    json_decref(root);
    return 0;
}

/*
  {"responseData":   {
  "results":[
  {
  "GsearchResultClass":"GwebSearch",
  "unescapedUrl":       
  "http://the-edge.blogspot.com/2009/08/going-retro-re-adopting-emacs.html",
  "url":"http://the-edge.blogspot.com/2009/08/going-retro-re-adopting-emacs.html",
  "visibleUrl":"the-edge.blogspot.com",
  "cacheUrl":"http://www.google.com/search?q\u003dcache:Kh-BN4cSqywJ:the-edge.blogspot.com",
  "title":"Postcards from the Bleeding Edge: Going retro, re-adopting Emacs!",
  "titleNoFormatting":"Postcards from the Bleeding Edge: Going retro, re-adopting Emacs!",
  "content":"Aug 10, 2009 \u003cb\u003e...\u003c/b\u003e G \u0026quot;* [[\u003cb\u003egnugol\u003c/b\u003e:%^{Keywords}]]\u0026quot; \u0026quot;~/org/\u003cb\u003egnugol\u003c/b\u003e.org\u0026quot; \u0026quot;Search Later\u0026quot;) \u003cb\u003e...\u003c/b\u003e n %u\u0026quot; \u0026quot;~/  org/\u003cb\u003eipv6\u003c/b\u003e.org\u0026quot; \u0026quot;Notes\u0026quot;) (\u0026quot;Vocab\u0026quot; ?v \u0026quot;** [[dict:%^{Word?}]]\\n%? \u003cb\u003e...\u003c/b\u003e"
  }
  ],
  "cursor":{
  "pages":[ {"start":"0","label":1} ],
  "estimatedResultCount":"1",
  "currentPageIndex":0,
"moreResultsUrl":"http://www.google.com/search?oe\u003dutf8\u0026ie\u003dutf8\u0026source\u003duds\u0026start\u003d0\u0026hl\u003den\u0026q\u003dipv6+gnugol"}}, 
 "responseDetails": null, 
 "responseStatus": 200}

 */


/* So, basically, inside of the results array, we want the url, title, and content for each */

// FIXME, add url encode
// FIXME UTF-8
// FIXME snprintf

int main(int argc, char **argv) { 
  char basequery[URL_SIZE];
  char qstring[URL_SIZE]; 
  setup(basequery);
  
  switch(argc) {
  case 1: usage("keyword keyword keyword"); break;
  case 2: sprintf(qstring, "%s&q=%s", basequery, argv[1]); break; 
  case 3: sprintf(qstring, "%s&q=%s%%20%s", basequery, argv[1], argv[2]); break; 
  case 4: sprintf(qstring, "%s&q=%s%%20%s%%20%s", basequery, argv[1], argv[2], argv[3]); break; 
  default: usage("This is only a test proggie, give me a break"); 
  }
  
  getresult(qstring,FORMATDEFAULT);

  return 0;
}
