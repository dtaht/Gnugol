/* This file implements a gnugol -> opensearch api -> gnugol json translator plugin */

#include <stddef.h>
#include <errno.h>
#include "query.h"

int setup(QueryOptions_t *q,char *string,size_t lenstr)
{
  return EOPNOTSUPP;
}

int results(QueryOptions_t *q,char *urltxt,size_t lenurl)
{
  return EOPNOTSUPP;
}

