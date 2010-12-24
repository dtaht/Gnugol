#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "query.h"
#include "formats.h"
#include "gnugol_engines.h"
#include "engines.h"

struct engines { 
  char *id;
  gnugol_engine e;
};


static int no_engine(QueryOptions_t *q) {
  GNUGOL_OUTE(q,"No engine\n");
  return(-1);
}

static struct engines e[] = { 
  { "google", engine_googlev1 },
  { "bing", engine_bing },
  { "wikipedia", engine_wikipedia },
  { "dummy", engine_dummy },
  { NULL, no_engine }
};

gnugol_engine gnugol_get_engine(QueryOptions_t *q) {
  for (int i = 0; e[i].id != NULL; i++) {
    if(strcmp(e[i].id,q->engine_name) == 0) return e[i].e;
  }
  return &no_engine;
}

/*

Can you tell what the following declaration means?

 
  void (*p[10]) (void (*)() );

Only few programmers can tell that p is an "array of 10 pointers to a function returning void and taking a pointer to another function that returns void and takes no arguments." The cumbersome syntax is nearly indecipherable. However, you can simplify it considerably by using typedef declarations. First, declare a typedef for "pointer to a function returning void and taking no arguments" as follows:

 
  typedef void (*pfv)();

Next, declare another typedef for "pointer to a function returning void and taking a pfv" based on the typedef we previously declared:

 
  typedef void (*pf_taking_pfv) (pfv);

Now that we have created the pf_taking_pfv typedef as a synonym for the unwieldy "pointer to a function returning void and taking a pfv", declaring an array of 10 such pointers is a breeze:

 
  pf_taking_pfv p[10];

*/
