#ifndef _gnugol_engines
#define _gnugol_engines 1
#define GNUGOL_SHAREDLIBDIR "/var/lib/gnugol"
#define GNUGOL_DECLARE_ENGINE(type,name) gnugol_engine_##type##_##name

typedef struct ggengine
{
  Node         node;
  const char  *name;
  int        (*setup)  (QueryOptions_t *);
  int        (*results)(QueryOptions_t *);
} *GnuGoldEngine;


// FIXME: Return a ptr to an opaque engine func instead of an offset
// into the engines table

typedef int (*gnugol_engine)(QueryOptions_t *q);
extern gnugol_engine get_engine(QueryOptions_t *q);

// This one does dynamic loading

extern int gnugol_query_engine(QueryOptions_t *q);

extern int gnugol_read_key(char *const,size_t *const,const char *const);

#endif
