
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>
#include <iconv.h>

#include "nodelist.h"
#include "query.h"
#include "formats.h"
#include "gnugol_engines.h"

#ifndef __GNUC__
#  define __attribute__(x)
#endif

static int gnugol_default_setup		(QueryOptions_t *);
static int gnugol_default_search	(QueryOptions_t *);

/************************************************************************/

GnuGolEngine gnugol_engine_load(const char *name)
{
  GnuGolEngine engine;
  char          libname[FILENAME_MAX];

  assert(name != NULL);

  engine = malloc(sizeof(struct ggengine));
  if (engine == NULL)
    return NULL;

  snprintf(libname,sizeof(libname),GNUGOL_SHAREDLIBDIR "%s." SO_EXT,name);
  engine->lib = dlopen(libname,RTLD_LAZY | RTLD_GLOBAL);

  if (engine->lib == NULL)
  {
    free(engine);
    return NULL;
  }

  engine->setup = dlsym(engine->lib,"setup");
  if (engine->setup == NULL)
    engine->setup = gnugol_default_setup;

  engine->search = dlsym(engine->lib,"search");
  if (engine->search == NULL)
    engine->search = gnugol_default_search;

  engine->description = (const char *)dlsym(engine->lib,"description");
  if (engine->description == NULL)
    engine->description = "There is no description";

  engine->name = strdup(name);

  return engine;
}

/*************************************************************************/

int gnugol_engine_query(GnuGolEngine engine,QueryOptions_t *query)
{
  int rc;

  assert(engine != NULL);
  assert(query  != NULL);

  rc = (*engine->setup)(query);
  if (rc < 0)
  {
    GNUGOL_OUTW(query,"%s: Went boom on setup\n",engine->name);
    return rc;
  }

  return (*engine->search)(query);
}

/************************************************************************/

void gnugol_engine_unload(GnuGolEngine engine)
{
  assert(engine != NULL);

  free((void *)engine->name);
  dlclose(engine->lib);
  free(engine);
}

/*************************************************************************/

int gnugol_read_key(
	char       *const key,
	size_t     *const pksize,
	const char *const keyfile
)
{
  char     path[FILENAME_MAX];
  char    *home;
  int      fd;
  ssize_t  size;
  int      err;

  assert(key     != NULL);
  assert(pksize  != NULL);
  assert(*pksize >  0);
  assert(keyfile != NULL);

  home = getenv("HOME");
  if (home == NULL)
    home = "";

  snprintf(path,sizeof(path),"%s/%s",home,keyfile);
  fd = open(path,O_RDONLY);
  if (fd == -1)
    return errno;

  size = read(fd,key,*pksize);
  err  = errno;
  close(fd);

  if (size == -1)
    return errno;

  if (size == 0)
    return ENODATA;

  while((size > 0) && ((key[size - 1] == ' ') || (key[size - 1] == '\n')))
    size--;

  if (size == 0)
    return ENODATA;

  key[size] = '\0';

  *pksize = size;
  return 0;
}

/***********************************************************************/

static int gnugol_default_setup(QueryOptions_t *query __attribute__((unused)))
{
  return 0;
}

/**********************************************************************/

static int gnugol_default_search(QueryOptions_t *query __attribute__((unused)))
{
  return 0;
}

/***********************************************************************/

