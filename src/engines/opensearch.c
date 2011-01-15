/* This file implements a gnugol -> opensearch api -> gnugol json translator plugin */

#include <stddef.h>
#include <errno.h>
#include <iconv.h>
#include "query.h"

int setup(QueryOptions_t *q)
{
  return EOPNOTSUPP;
}

int search(QueryOptions_t *q)
{
  return EOPNOTSUPP;
}

