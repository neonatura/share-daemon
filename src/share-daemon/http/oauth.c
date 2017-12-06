
/*
 *  Copyright 2016 Neo Natura 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/neonatura/share)
 *        
 *  The Share Library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version. 
 *
 *  The Share Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.
 */  

#include "sharedaemon.h"

int oauth_scope_bits(char *scope_str)
{
  char buf[1024];
  char *tok;
  int flag;

  if (!scope_str || !*scope_str)
    return (0);

  memset(buf, 0, sizeof(buf));
  strncpy(buf, scope_str, sizeof(buf) - 1);

  flag = 0;
  tok = strtok(buf, ", ");
  while (tok) {
    if (0 == strcasecmp(tok, "location")) {
      flag |= OAUTH_SCOPE_LOCATION;
    } else if (0 == strcasecmp(tok, "info")) {
      flag |= OAUTH_SCOPE_INFO;
    } else if (0 == strcasecmp(tok, "media")) {
      flag |= OAUTH_SCOPE_MEDIA;
    } else if (0 == strcasecmp(tok, "scan")) {
      flag |= OAUTH_SCOPE_SCAN;
    } else if (0 == strcasecmp(tok, "wallet")) {
      flag |= OAUTH_SCOPE_WALLET;
    }

    tok = strtok(NULL, ", ");
  }

  return (flag);
}


const char *oauth_scope_label(int scope)
{

  switch (scope) {
    case OAUTH_SCOPE_INFO:
      return "info";
    case OAUTH_SCOPE_LOCATION:
      return "location";
    case OAUTH_SCOPE_MEDIA:
      return "media";
    case OAUTH_SCOPE_SCAN:
      return "scan";
    case OAUTH_SCOPE_WALLET:
      return "wallet";
  }

  return "";
}
