/* This engine implements a:
   gnugol -> stackapps web api -> gnugol json translator engine

json API doc at:
   http://api.stackoverflow.com/1.0/help/method?method=search

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <jansson.h>
#include <ctype.h>
#include <curl/curl.h>
#include "query.h"
#include "utf8.h"
#include "handy.h"
#include "formats.h"
#include "gnugol_engines.h"

#define TEMPLATE  "http://api.stackoverflow.com/1.0/search"
#define LICENSE_URL "http://stackapps.com/apps/register"
#define TOU ""

static const char *safe_map[] = {
	 "off", "moderate", "active", NULL
};

int setup(QueryOptions_t *q) {
  char   string[URL_SIZE];
  char   key   [256];
  size_t size;
  char   hl[8];
  char   lr[8];

  hl[0] = lr[0] = '\0';

  if(q->debug > 9) GNUGOL_OUTW(q,"stackapps: Entering Setup\n");

  size = sizeof(key);
  if (gnugol_read_key(key,&size,".stackappkey") != 0)
  {
    GNUGOL_OUTE(q,"A license key to search stackapps is recommended. "
		  "You can get one from: %s",LICENSE_URL);
    size = 0;
  }

  if(q->safe < 0) q->safe = 0;
  if(q->safe > 2) q->safe = 2;
  if(q->debug > 5) GNUGOL_OUTW(q,"stackapps: safesearch=%s\n",safe_map[q->safe]);
  if(q->nresults > 100) q->nresults = 100; // stackapps enforced
  if(q->debug > 4) GNUGOL_OUTW(q,"stackapps: KEYWORDS = %s\n", q->keywords);

/*
   While we can't be sure the user's two letter code is supported
   we CAN try harder to not mess up than this.
*/

  if((q->input_language[0] != '\0') &&
	  (isalpha(q->input_language[0])) &&
	  (isalpha(q->input_language[1]))) {
	  hl[0] = '&';
	  hl[1] = 'h';
	  hl[2] = 'l';
	  hl[3] = '=';
	  hl[4] = q->input_language[0];
	  hl[5] = q->input_language[1];
	  hl[6] = '\0';
  }
  if((q->output_language[0] != '\0') &&
	  (isalpha(q->output_language[0])) &&
	  (isalpha(q->output_language[1]))) {
	  lr[0] = '&';
	  lr[1] = 'l';
	  lr[2] = 'r';
	  lr[3] = '=';
	  lr[4] = q->output_language[0];
	  lr[5] = q->output_language[1];
	  lr[6] = '\0';
  }

// FIXME:  Note that position is a function of the pagesize
// pagesize = 10, and position 1

// FIXME: Secondly, this is not a keyword search but a phrase search
//        We could fix things by looking for a set of stuff commonly
//        tagged so "linux enable ECN" would become tagged "linux"
///       enable ECN
// FIXME: This does not specify a date range, which might be useful

// FIXME: Also, sorting by reputation looks like a good idea
//        As does actually making a query for the actual question/answer

// FIXME: At present, this takes 7-10 seconds to execute. Maybe
//        the sort by votes is the cause?


  if (size == 0)
	  size = snprintf(string,URL_SIZE,
			  "%s?pagesize=%d&intitle=%s",
			  TEMPLATE,
			  q->nresults,
			  q->keywords);
  else
	  size = snprintf(string,URL_SIZE,
			  "%s?key=%s&pagesize=%d&intitle=%s",
			  TEMPLATE,key,
			  q->nresults,
			  q->keywords);

  if (size > sizeof(q->querystr))
  {
    GNUGOL_OUTE(q,"Size of URL exceeds space set aside for it");
    return -1;
  }

  strcpy(q->querystr,string);
  if(q->debug > 9) GNUGOL_OUTW(q,"Exiting Setup\n");
  return (int)size;
}

// turn quotes back into quotes and other utf-8 stuff
// FIXME: Error outs cause a memory leak from "root"
// use thread local storage? or malloc for the buffer
// FIXME: do fuller error checking
//        Fuzz inputs!
// Maybe back off the number of results when we overflow the buffer
// This engine takes advantage (abuses!) the CPP pasting tokens
// with a couple macros to make the interface to json a 1 to 1 relationship
// The code is delightfully short this way.

/*
  Not for the first time, having some clean way to mark reputation up or down
  would be nice

      "title": {
        "description": "title of this post, in plaintext",
        "values": "string",
        "optional": false,
        "suggested_buffer_size": 200
      },
      "body": {
        "description": "body of this post, rendered as HTML",
        "values": "string",
        "optional": true,
        "suggested_buffer_size": 30000
      },
      "view_count": {
        "description": "number of times this post has been viewed",
        "values": "32-bit signed integer",
        "optional": false
      },
      "score": {
        "description": "score of this post",
        "values": "32-bit signed integer",
        "optional": false
      },

AND it would be kind of cool to take the comments and have them in the outline

  "comments": [
        {
          "comment_id": {
            "description": "id of the comment",
            "values": "32-bit signed integer",
            "optional": false
          },
          "creation_date": {
            "description": "creation date of the comment",
            "values": "unix epoch date, range [0, 253,402,300,799]",
            "optional": false
          },
          "owner": {
            "user_id": {
              "description": "id of the user",
              "values": "32-bit signed integer",
              "optional": false
            },
            "user_type": {
              "description": "type of the user",
              "values": "one of anonymous, unregistered, registered, or moderator",
              "optional": false
            },
            "display_name": {
              "description": "displayable name of the user",
              "values": "string",
              "optional": false,
              "suggested_buffer_size": 40
            },
            "reputation": {
              "description": "reputation of the user",
              "values": "32-bit signed integer",
              "optional": false
            },
            "email_hash": {
              "description": "email hash, suitable for fetching a gravatar",
              "values": "string",
              "optional": false,
              "suggested_buffer_size": 32
            }
          },
          "reply_to_user": {
            "user_id": {
              "description": "id of the user",
              "values": "32-bit signed integer",
              "optional": false
            },
            "user_type": {
              "description": "type of the user",
              "values": "one of anonymous, unregistered, registered, or moderator",
              "optional": false
            },
            "display_name": {
              "description": "displayable name of the user",
              "values": "string",
              "optional": false,
              "suggested_buffer_size": 40
            },
            "reputation": {
              "description": "reputation of the user",
              "values": "32-bit signed integer",
              "optional": false
            },
            "email_hash": {
              "description": "email hash, suitable for fetching a gravatar",
              "values": "string",
              "optional": false,
              "suggested_buffer_size": 32
            }
          },

*/

// Questions/Answers/Comments
// score looks useful
// Answers -> title
// Answers -> body
// In thinking about this, maybe doing a google site search
// AND then following it up with a search for the ids involved
// Would build up a useful faq-like result

int search(QueryOptions_t *q) {
    unsigned int i;
    char *text;
    json_t *root,*questions;
    json_error_t error;
    if(q->debug) GNUGOL_OUTW(q,"%s: trying url: %s\n", q->engine_name, q->querystr);

    text = jsonrequest(q->querystr);
    if(!text) {
      GNUGOL_OUTE(q,"url failed to work: %s", q->querystr);
      return -1;
    }

    root = json_loads(text, &error);
    free(text);

    if(!root)
    {
        GNUGOL_OUTE(q,"error: on line %d: %s\n", error.line, error.text);
        return -1;
    }

    GETARRAY(root,questions);
    gnugol_header_out(q);

//  Maybe use the score or whether the answer is known for the title? 
//  Or the date?
//  Or the accepted_answer id, if it exists, rather than referencing
//  the question

    for(i = 0; i < json_array_size(questions); i++)
    {
      json_t *question, *question_answers_url, *title;
      json_t *score;
      char buffer[1024];
      char buffer2[1024];
      GETARRAYIDX(questions,question,i);
      GETSTRING(question,question_answers_url);
      GETSTRING(question,title);
      GETNUMBER(question,score);
//    GETNUMBER(question,accepted_answer_id);
//    GETSTRING(question,body); // doesn't exist, although documented
//    if(strlen(accepted_answer_id) > 0) {
//    snprintf(buffer,1024,"http://stackoverflow.com%s",jsv(question_answers_url));
//    }  else {
      snprintf(buffer,1024,"http://stackoverflow.com%s",jsv(question_answers_url));
//    }

//    url, url title, description, snippet would be easier to read
//    but that involves rewriting the formatter
      snprintf(buffer2,1024,"%p",(void *)jsv(score));
      gnugol_result_out_long(q,buffer,buffer2,jsv(title),NULL);
//    gnugol_result_out(q,buffer,jsv(title),NULL);

/*    It is actually, really, really annoying to not even
      have a portion of the answer in the query.

      I'm almost annoyed enough to thread the engine, & issue
      the next set of results for the queries in parallel,
      and then fill in the blanks as per the below. However,
      that may not be necessary, perhaps this is the right api
      call to "fill in the blanks".

http://api.stackoverflow.com/1.0/help/method?method=questions/{id}

      gnugol_result_out(q,NULL,"Answers",NULL);
           GETARRAYIDX(question,answers,i);
      q->indent++;
      for(int j = 0; j < json_array_size(answers); j++) {
      GETARRAYIDX(answers,answer,j);
//    GETSTRING(answer,url);
      GETSTRING(answer,title);
      GETSTRING(answer,body);
      gnugol_result_out(q,NULL,jsv(title),jsv(body));
      }
      q->indent--;
      */

    }

    gnugol_footer_out(q);

    // FIXME: Go recursive if we overflowed the buffer

    json_decref(root);
    return 0;
}

