/* This file implements a gnugol -> dns api -> gnugol json translator plugin */

#include <stddef.h>
#include <errno.h>
#include "query.h"
#include "gnugol_engines.h"

#ifndef __GNUC__
#  define __attribute__(x)
#endif

int GNUGOL_DECLARE_ENGINE(setup,dns) (QueryOptions_t *q  __attribute__((unused)))
{
  return EOPNOTSUPP;
}

int GNUGOL_DECLARE_ENGINE(search,dns) (QueryOptions_t *q  __attribute__((unused)))
{
  return EOPNOTSUPP;
}

