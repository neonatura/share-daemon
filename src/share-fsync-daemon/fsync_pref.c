
/*
 *  Copyright 2014 Neo Natura 
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

#include "fsync.h"

static shmap_t *fsync_preferences;
static char *fsync_preferences_data;
 
int fsync_shpref_init(int uid)
{
  shmap_t *h;
  struct stat st;
  char *path;
  char *data;
  shkey_t *key;
  size_t data_len;
  size_t len;
  int err;
  int b_of;

  h = shmap_init();
  if (!h)
    return (SHERR_NOMEM);


  path = shpref_path(uid);
  err = shfs_read_mem(path, &data, &data_len);
  if (!err) { /* file may not have existed. */
    shbuf_t *buff = shbuf_map(data, data_len);
    shmap_load(h, buff);
    free(buff);
  }

  free(key);

  fsync_preferences = h;
  fsync_preferences_data = data;

  return (0);
}

const char *fsync_shpref_get(char *pref, char *default_value)
{
  static char ret_val[SHPREF_VALUE_MAX+1];
  char tok[SHPREF_NAME_MAX + 16];
  shkey_t *key;
  char *str;
  int err;

  if (!fsync_preferences)
    return (default_value);

  err = shpref_init();
  if (err)
    return (default_value);

  memset(tok, 0, sizeof(tok));
  strncpy(tok, pref, SHPREF_NAME_MAX);
  key = ashkey_str(tok);
  str = shmap_get_str(fsync_preferences, key);

  memset(ret_val, 0, sizeof(ret_val));
  if (!str) {
    if (default_value)
      strncpy(ret_val, default_value, sizeof(ret_val) - 1);
  } else {
    strncpy(ret_val, str, sizeof(ret_val) - 1);
  }

  return (ret_val);
}

void fsync_shpref_free(void)
{

  if (!fsync_preferences)
    return;

  shmap_free(&fsync_preferences);
  fsync_preferences = NULL;

  free(fsync_preferences_data);
  fsync_preferences_data = NULL;
}
