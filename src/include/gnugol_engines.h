#ifndef _gnugol_engines
#define _gnugol_engines 1

#ifdef DEBUG_SHAREDLIBS
#  define GNUGOL_SHAREDLIBDIR "../engines"
#else
#  define GNUGOL_SHAREDLIBDIR "/var/lib/gnugol"
#endif

typedef struct ggengine
{
  Node         node;
  void        *lib;
  const char  *name;
  int        (*setup) (QueryOptions_t *);
  int        (*search)(QueryOptions_t *);
} *GnuGolEngine;

GnuGoldEngine	gnugol_engine_load	(const char *);
int		gnugol_engine_query	(GnuGolEngine,QueryOptions_t *);
void		gnugol_engine_unload	(GnuGolEngine);
int		gnugol_read_key		(char *const,size_t *const,const char *const);

#endif
