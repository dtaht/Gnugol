#include "gnugol_engines.h"

// FIXME: maybe rethink this to just return a ptr
// to an opaque engine type

extern int engine_googlev1(QueryOptions_t *q);
extern int engine_bing(QueryOptions_t *q);
extern int engine_dummy(QueryOptions_t *q);

