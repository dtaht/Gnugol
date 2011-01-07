/* This file implements a gnugol -> dns api -> gnugol json translator plugin */

#include <stddef.h>
#include <errno.h>
#include "query.h"
#include "gnugol_engines.h"

int setup(QueryOptions_t *q  __attribute__((unused)))
{
  return EOPNOTSUPP;
}

int search(QueryOptions_t *q  __attribute__((unused)))
{
  return EOPNOTSUPP;
}

