#ifndef _gnugol_engines
#define _gnugol_engines 1
// FIXME: Return a ptr to an opaque engine func instead of an offset
// into the engines table

typedef int (*gnugol_engine)(QueryOptions_t *q);

extern gnugol_engine get_engine(QueryOptions_t *q);

#endif
