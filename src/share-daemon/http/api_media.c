
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



int api_media_list_cb(shfs_ino_t *file, shjson_t *result)
{

  shjson_str_add(result, NULL, shfs_filename(file));
  shjson_num_add(result, NULL, shfs_size(file));

//  fprintf(sharetool_fout, "%-*.*s%s\n", level, level, "", shfs_inode_print(file));

  return (0);
}


int api_media_list(shjson_t *reply, shjson_t *param, shmap_t *sess)
{
  struct shstat st;
  SHFL *file;
  shfs_t *tree;
  shjson_t *result;
  char dirpath[PATH_MAX+1];
  char fname[PATH_MAX+1];
  char *path;
  int level;
  int err;

  path = shjson_array_str(param, NULL, 0);
  if (!path)
    return (SHERR_INVAL);

  memset(fname, 0, sizeof(fname));
  strncpy(fname, basename(path), sizeof(fname)-1);

  memset(dirpath, 0, sizeof(dirpath));
  strncpy(dirpath, path, strlen(path) - strlen(fname));

  tree = shfs_uri_init(dirpath, 0, &file);
  if (!tree)
    return (SHERR_NOENT);

  err = shfs_fstat(file, &st);
  if (err)
    return (err);

  level = 0;
  result = shjson_array_add(reply, "result");
  shfs_list_cb(file, shjson_array_astr(param, NULL, 0),
    SHLIST_F(api_media_list_cb), result);

  return (0);
}

int api_media_read(shjson_t *reply, shjson_t *param, shmap_t *sess)
{
  return (SHERR_OPNOTSUPP);
}

int api_media_write(shjson_t *reply, shjson_t *param, shmap_t *sess)
{
  return (SHERR_OPNOTSUPP);
}


