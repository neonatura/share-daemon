
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



int share_sync_init(fuser_t *u, sync_t *sync, void *unused)
{
  return (0);
} 

int share_sync_term(fuser_t *u, sync_t *sync, void *unused)
{
  return (0);
}

int share_sync_watch(fuser_t *u, sync_t *sync, const char *path)
{
  int wd = 0;

  return (wd);
}

int share_sync_poll_wait(fuser_t *u, sync_t *sync, double *to_p)
{
  return (0);
}

int share_sync_poll(fuser_t *u, sync_t *sync, double *to_p)
{

  return (0);
}

int share_sync_remove(fuser_t *u, sync_t *sync, sync_ent_t *ent)
{

#ifdef linux
  inotify_rm_watch(sync->sync_fd, ent->id);
#endif
  return (0);
}

int share_sync_read(fuser_t *u, sync_t *sync, void *unused)
{
  return (0);
}

int share_sync_write(fuser_t *u, sync_t *sync, sync_ent_t *ent)
{
fprintf(stderr, "DEBUG: share_sync_write()\n");
  return (0);
}

