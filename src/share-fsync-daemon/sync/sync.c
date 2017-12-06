
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


sync_op_t sync_op_table[MAX_SYNC_FS] = {
  { }, /* FS_NONE */

  { SYNCF(share_sync_init), SYNCF(share_sync_term), SYNCF(share_sync_watch), SYNCF(share_sync_poll), SYNCF(share_sync_remove), SYNCF(share_sync_read), SYNCF(share_sync_write) },  /* FS_SHARE */

#ifdef linux
  { SYNCF(linux_sync_init), SYNCF(linux_sync_term), SYNCF(linux_sync_watch), SYNCF(linux_sync_poll), SYNCF(linux_sync_remove), SYNCF(linux_sync_read), SYNCF(linux_sync_write) },  /* FS_LINUX */
#else
  { }, /* FS_LINUX n/a */
#endif
};

sync_ent_t *sync_ent_id(sync_t *sync, int ent_id)
{
  sync_ent_t *ent;

  for (ent = sync->ent_list; ent; ent = ent->next) {
    if (!(ent->flags & FSENT_ACTIVE))
      continue;
    if (ent->id == ent_id)
      return (ent);
  }

  return (NULL);
}

sync_ent_t *sync_ent_path(sync_t *sync, const char *path)
{
  sync_ent_t *ent;

  for (ent = sync->ent_list; ent; ent = ent->next) {
    if (0 == strcmp(ent->path, path))
      return (ent);
  }

  return (NULL);
}

sync_ent_t *sync_ent_init(sync_t *sync, int id, const char *path)
{
  sync_ent_t *ent;

/* TODO: check for non-active unused */

  /* allocate entity */
  ent = (sync_ent_t *)calloc(1, sizeof(sync_ent_t));
  if (!ent)
    return (NULL);

  /* set attributes */
  strcpy(ent->path, path);
  ent->id = id;
  ent->flags |= FSENT_ACTIVE;

  /* add to entity list */
  ent->next = sync->ent_list;
  sync->ent_list = ent;

  return (ent);
}

void sync_ent_free(sync_ent_t **ent_p)
{
  sync_ent_t *ent;

  if (!ent_p)
    return;

  ent = *ent_p;
  *ent_p = NULL;

  if (ent->iobuff)
    shbuf_free(&ent->iobuff);

  free(ent);
}


int sync_watch(fuser_t *user, sync_t *sync, const char *in_path)
{
  sync_ent_t *ent;
  char path[PATH_MAX+1];
  int err;
  int id;

  memset(path, 0, sizeof(path));
  strncpy(path, in_path, sizeof(path)-1);

  if (strlen(path) && path[strlen(path)-1] == '/')
    path[strlen(path)-1] = '\000';

  if (!sync->op->watch)
    return (SHERR_OPNOTSUPP);

  /* register path */
  id = sync->op->watch(user, sync, path);
  if (id < 0)
    return (err);

  /* allocate entity */
  ent = sync_ent_init(sync, id, path);
  if (!ent)
    return (SHERR_NOMEM);

  return (0);
} 

int sync_init(fuser_t *user, sync_t *sync, int fs_type, const char *path)
{
  int err;

  if (fs_type <= 0 || fs_type >= MAX_SYNC_FS)
    return (SHERR_INVAL);

  memset(sync, 0, sizeof(sync_t));
  sync->sync_type = fs_type;
  sync->op = &sync_op_table[fs_type];
  strncpy(sync->sync_path, path, sizeof(sync->sync_path)-1);
 
  if (sync->op->init) {
    err = sync->op->init(user, sync, NULL);
    if (err) {
      sync_term(user, sync);
      return (err);
    }
  }

#if 0
  err = sync_watch(sync, path);
  if (err) {
    sync_term(sync);
    return (err);
  }
#endif
fprintf(stderr, "DEBUG: sync_init: path '%s'\n", sync->sync_path);  

  return (0);
}


void sync_remove(fuser_t *user, sync_t *sync, sync_ent_t *ent)
{
  int err;

  if (sync->op->remove) {
    sync->op->remove(user, sync, ent);
  }

  ent->id = 0;
  ent->flags &= ~FSENT_ACTIVE;
}

int sync_poll(fuser_t *user, sync_t *sync, double to)
{
  int err;

  if (sync->op->poll) {
    err = sync->op->poll(user, sync, &to);
    if (err)
      return (err);
  }

  return (0);
}

int sync_term(fuser_t *user, sync_t *sync)
{
  sync_ent_t *ent_next;
  sync_ent_t *ent;

  if (sync->op->term)
    sync->op->term(user, sync, NULL);

  for (ent = sync->ent_list; ent; ent = ent_next) {
    ent_next = ent->next;

    sync_ent_free(&ent);
  }
  sync->ent_list = NULL;

  sync->sync_fd = 0;
}

void sync_ent_copy(sync_ent_t *s_ent, sync_ent_t *d_ent)
{
  shbuf_clear(d_ent->iobuff);
  shbuf_append(d_ent->iobuff, s_ent->iobuff);
}

int sync_ent_compare(sync_ent_t *s_ent, sync_ent_t *d_ent)
{

/* DEBUG: TODO: may need to fill X->info */

  if (s_ent->info.st_size != d_ent->info.st_size)
    return (FALSE);

  if (s_ent->info.st_mtime != d_ent->info.st_mtime)
    return (FALSE);

  return (TRUE);
}

int sync_relay_update(fuser_t *user, sync_t *sync, sync_ent_t *ent)
{
  shbuf_t *in_buff;
  sync_ent_t *d_ent;
  sync_t *d_sync;
  int f_create;
  int err;
  int idx;

  if (!sync->op->read)
    return (SHERR_OPNOTSUPP);

  f_create = FALSE;

  if (sync == &user->lcl_sync)
    d_sync = &user->rem_sync;
  else if (sync == &user->rem_sync)
    d_sync = &user->lcl_sync;
  else
    return (SHERR_INVAL);

  d_ent = sync_ent_path(d_sync, ent->hpath); 
  if (!d_ent) {
    /* d_ent = .. */
    f_create = TRUE;
  } else {
    f_create = (FALSE == sync_ent_compare(ent, d_ent));
  }

  if (f_create) {
    err = sync->op->read(user, sync, ent); 
    if (err)
      return (err);

    sync_ent_copy(ent, d_ent);
    err = sync->op->write(user, d_sync, d_ent);
    if (err)
      return (err);
  }

  return (0);
}


