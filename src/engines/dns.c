/* 
Copyright (C) 2011 Michael D. Täht

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see http://www.gnu.org/licenses/
 */

/* This file implements a gnugol -> dns api -> gnugol json translator plugin */

#include <stddef.h>
#include <errno.h>
#include "query.h"
#include "gnugol_engines.h"

const char description[] = "Eventually there will be a module to query DNS";

int setup(QueryOptions_t *q  __attribute__((unused)))
{
  return EOPNOTSUPP;
}

int search(QueryOptions_t *q  __attribute__((unused)))
{
  return EOPNOTSUPP;
}

