/* Xapian's omega does support the opensearch API, but I'm thinking I want more of a direct interface to omega */

#include <stddef.h>
#include <errno.h>
#include "query.h"

int GNUGOL_DECLARE_ENGINE(setup,omega) (QueryOptions_t *q)
{
  return EOPNOTSUPP;
}

int GNUGOL_DECLARE_ENGINE(search,omega) (QueryOptions_t *q)
{
  return EOPNOTSUPP;
}

