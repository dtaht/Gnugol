#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "query.h"
#include "formats.h"
#include "gnugol_engines.h"
#include "engines.h"

static void *gnugol_try_openlib(QueryOptions_t *q) {
  void *lib;
  char libname[FILENAME_MAX];
  
  snprintf(libname,FILENAME_MAX, GNUGOL_SHAREDLIBDIR "/%s.so",q->engine_name);
  lib = dlopen(libname,RTLD_LAZY | RTLD_GLOBAL);

#ifndef PRODUCTION  
  if (lib == NULL) {
    GNUGOL_OUTW(q,"%s(1): Not in default location, error: %s\n",q->engine_name,dlerror());
    snprintf(libname,FILENAME_MAX,"../engines/%s.so",q->engine_name);
    lib = dlopen(libname,RTLD_LAZY | RTLD_GLOBAL);
  }
#endif

  if (lib == NULL)
  {
    GNUGOL_OUTE(q,"%s(1): %s\n",q->engine_name,dlerror());
    return NULL;
  }
  return lib;
}

// The external namespace is of the format
// gnugol_engine_type_engine
// FIXME: There is a major security hole. We really want to 
// sanitize our inputs, otherwise ANY shared library could be
// run

#define ENGINE_FORMAT "gnugol_engine_%s_%s"
int gnugol_query_engine(QueryOptions_t *q)
{
  void *lib;
  char funcname[255];
  int (*setup)(QueryOptions_t *);
  int (*results)(QueryOptions_t *);
  int  rc;
  if(q->debug)
    GNUGOL_OUTW(q,"Engine selected: %s\n",q->engine_name);

  lib = gnugol_try_openlib(q);

  if(q->debug)
    GNUGOL_OUTW(q,"%s: trying to acquire shared lib\n",q->engine_name);

  if(lib == NULL) { 
    GNUGOL_OUTE(q,"%s: failed to acquire shared lib\n",q->engine_name);
    return(-1);
  }
  sprintf(funcname,ENGINE_FORMAT,"setup",q->engine_name);
  setup = dlsym(lib,funcname);	/* known warning here, POSIX allows this */

  if(q->debug)
    GNUGOL_OUTW(q,"%s: getting setup function \n",q->engine_name);

  if (setup == NULL)
  {
    GNUGOL_OUTE(q,"%s(2): %s\n",q->engine_name,dlerror());
    dlclose(lib);
    return -1;
  }
  
  if(q->debug)
    GNUGOL_OUTW(q,"%s: got setup function \n",q->engine_name);
  sprintf(funcname,ENGINE_FORMAT,"search",q->engine_name);
  results = dlsym(lib,funcname);	/* known warning here, POSIX allows this */
  if (results == NULL)
  {
    GNUGOL_OUTE(q,"%s(3): %s\n",q->engine_name,dlerror());
    dlclose(lib);
    return -1;
  }

  if(q->debug)
    GNUGOL_OUTW(q,"%s: shared libs are live\n",q->engine_name);
  
  rc = (*setup)(q);
  if (rc < 0)
  { 
    GNUGOL_OUTW(q,"%s: Went boom on setup\n",q->engine_name);
    dlclose(lib);
    return rc;
  }
  if(q->debug)
    GNUGOL_OUTW(q,"%s: trying query\n",q->engine_name);
  
  rc = (*results)(q);
  
  dlclose(lib);
  return rc;
}


// This was my first attempt and it appears to be wrong

#ifdef GNUGOL_STATIC

struct engines { 
  char *id;
  gnugol_engine e;
};

static int no_engine(QueryOptions_t *q) {
  GNUGOL_OUTE(q,"No engine\n");
  return(-1);
}


static struct engines e[] = { 
  { "google", GNUGOL_DECLARE_ENGINE(search,google) },
  { "bing",   GNUGOL_DECLARE_ENGINE(search,bing) },
  { "wikipedia", GNUGOL_DECLARE_ENGINE(search,wikipedia) },
  { "dummy", GNUGOL_DECLARE_ENGINE(search,dummy) },
  { NULL, no_engine }
};


gnugol_engine gnugol_get_engine(QueryOptions_t *q) {
  for (int i = 0; e[i].id != NULL; i++) {
    if(strcmp(e[i].id,q->engine_name) == 0) return e[i].e;
  }
  return &no_engine;
}

#endif

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
