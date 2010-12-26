/* This file implements a gnugol -> dns api -> gnugol json translator plugin */

#include <stddef.h>
#include <errno.h>
#include "query.h"

#ifndef __GNUC__
#  define __attribute__(x)
#endif

int setup(QueryOptions_t *q  __attribute__((unused)))
{
  return EOPNOTSUPP;
}

int results(QueryOptions_t *q  __attribute__((unused)))
{
  return EOPNOTSUPP;
}

