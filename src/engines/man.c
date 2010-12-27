/* This file implements a gnugol -> manpage server -> gnugol json translator plugin */

#include <stddef.h>
#include <errno.h>
#include "query.h"

int GNUGOL_DECLARE_ENGINE(setup, man) (QueryOptions_t *q)
{
  return EOPNOTSUPP;
}

int GNUGOL_DECLARE_ENGINE(search,man) (QueryOptions_t *q)
{
  return EOPNOTSUPP;
}

