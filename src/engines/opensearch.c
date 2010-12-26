/* This file implements a gnugol -> opensearch api -> gnugol json translator plugin */

#include <stddef.h>
#include <errno.h>
#include "query.h"

int GNUGOL_DECLARE_ENGINE(setup,opensearch) (QueryOptions_t *q)
{
  return EOPNOTSUPP;
}

int GNUGOL_DECLARE_ENGINE(search,opensearch) (QueryOptions_t *q)
{
  return EOPNOTSUPP;
}

