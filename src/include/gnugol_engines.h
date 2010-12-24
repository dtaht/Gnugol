#ifndef _gnugol_engines
#define _gnugol_engines 1

// FIXME: Return a ptr to an opaque engine func instead of an offset
// into the engines table

extern int get_engine(QueryOptions *q);

#endif
