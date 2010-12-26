/* Xapian's omega does support the opensearch API, but I'm thinking I want more of a direct interface to omega */

#include <stddef.h>
#include <errno.h>
#include "query.h"

int setup(QueryOptions_t *q)
{
  return EOPNOTSUPP;
}

int results(QueryOptions_t *q)
{
  return EOPNOTSUPP;
}

