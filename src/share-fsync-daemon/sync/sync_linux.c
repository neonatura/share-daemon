
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

#ifdef linux

static int _linux_sync_scan(fuser_t *u, sync_t *sync, const char *root_path)
{
  DIR *dir;
  struct dirent *ent;
  char path[PATH_MAX+1];
  struct stat st;
  int err;

  err = sync_watch(u, sync, root_path);
  if (err) {
    closedir(dir);
    return (err);
  }

  dir = opendir(root_path);
  if (!dir)
    return (-errno);

  while ((ent = readdir(dir))) {
    if (0 == strcmp(ent->d_name, ".") ||
        0 == strcmp(ent->d_name, ".."))
      continue;

    sprintf(path, "%s/%s", root_path, ent->d_name);
    err = stat(path, &st);
    if (err)
      continue;
    if (!S_ISDIR(st.st_mode))
      continue;

    err = _linux_sync_scan(u, sync, path); 
    if (err) {
      closedir(dir);
      return (err);
    }
  }

  closedir(dir);
  return (0);
}

/* scan hierarchy for all directories */
static int linux_sync_scan(fuser_t *u, sync_t *sync)
{
  return (_linux_sync_scan(u, sync, sync->sync_path));
}

static int linux_sync_poll_event(fuser_t *u, sync_t *sync, struct inotify_event *event)
{
  sync_ent_t *ent;
  char path[PATH_MAX+1];
  int err;

  if (!event->len)
    return (0);

  ent = sync_ent_id(sync, event->wd);
  if (!ent) {
    fprintf(stderr, "DEBUG: unknown event wd %d\n", event->wd);
    return (SHERR_INVAL);
  }

  if (event->mask & IN_CREATE) {
    if ( event->mask & IN_ISDIR ) {
      printf( "The directory %s was created. [%s/%s]\n", event->name, ent->path, event->name );       
      sprintf(path, "%s/%s", ent->path, event->name);
      err = sync_watch(u, sync, path);
      fprintf(stderr, "DEBUG: linux_sync_poll_event: %d = linux_sync_watch(\"%s\")\n", err, path);
    }
    else {
      /* FILE CREATION */
      ent = sync_ent_path(sync, path); 
      if (ent)
        sync_relay_update(u, sync, ent);

fprintf(stderr, "DEBUG: CREATE: %s\n", event->name );
    }
  } else if (event->mask & IN_DELETE) {
    if ( event->mask & IN_ISDIR ) {
      printf( "The directory %s was deleted.\n", event->name );       
      sync_remove(u, sync, ent);
    } else {
      printf( "The file %s was deleted.\n", event->name );
    }
  } else if ( event->mask & IN_MODIFY ) {
    if ( event->mask & IN_ISDIR ) {
      printf( "The directory %s was modified.\n", event->name );
    }
    else {
      printf( "The file %s was modified.\n", event->name );
    }
  } else if (event->mask & IN_MOVED_FROM) {
    if ( event->mask & IN_ISDIR ) {
      printf( "The directory %s was moved from.\n", event->name );       
      sprintf(path, "%s/%s", ent->path, event->name);
      ent = sync_ent_path(sync, path); 
      if (ent) {
        sync_remove(u, sync, ent);
      }
    }
    else {
      printf( "The file %s was moved from.\n", event->name );
    }
  } else if (event->mask & IN_MOVED_TO) {
    if ( event->mask & IN_ISDIR ) {
      printf( "The directory %s was moved to.\n", event->name );       
      sprintf(path, "%s/%s", ent->path, event->name);
      err = sync_watch(u, sync, path);
      fprintf(stderr, "DEBUG: %d = linux_sync_watch('%s')\n", err, path);
    }
    else {
      printf( "The file %s was moved to.\n", event->name );
    }
  } else {
    fprintf(stderr, "DEBUG: unknown event mask %d\n", event->mask);
  }

  return (0);
}

int linux_sync_init(fuser_t *u, sync_t *sync, void *unused)
{
  int fd;

  fd = inotify_init1(O_NONBLOCK);
  if (fd == -1)
    return (-errno);

  sync->sync_fd = fd;

  return (linux_sync_scan(u, sync));
} 

int linux_sync_term(fuser_t *u, sync_t *sync, void *unused)
{
  sync_ent_t *ent;

  for (ent = sync->ent_list; ent; ent = ent->next) {
    if (ent->flags & FSENT_ACTIVE) 
      inotify_rm_watch(sync->sync_fd, ent->id);
  }

  close(sync->sync_fd);
  return (0);
}

int linux_sync_watch(fuser_t *u, sync_t *sync, const char *path)
{
  sync_ent_t *ent;
  int wd;

  wd = inotify_add_watch(sync->sync_fd, 
      path, IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVE); 
  if (wd == -1)
    return (-errno);

  return (wd);
}

int linux_sync_poll_wait(fuser_t *u, sync_t *sync, double *to_p)
{
  struct timeval tv;
  fd_set r_set;
  int err;

  if (*to_p < 0.0001)
    return (0); /* all done */ 

  FD_ZERO(&r_set);
  FD_SET(sync->sync_fd, &r_set);

  memset(&tv, 0, sizeof(tv));
  tv.tv_sec = (time_t)*to_p;
  tv.tv_usec = (*to_p - (double)tv.tv_sec) / 1000000;
  err = select(sync->sync_fd+1, &r_set, NULL, NULL, &tv);
  if (err < 0)
    return (-errno);

  *to_p = (double)tv.tv_sec + ((double)tv.tv_usec / 1000000);
  return (0);
}

int linux_sync_poll(fuser_t *u, sync_t *sync, double *to_p)
{
  char buff[4096];
  struct inotify_event *event = (struct inotify_event * )buff;
  size_t r_len;
  double to;
  int len;
  int err;
  int i;

  err = linux_sync_poll_wait(u, sync, to_p);
  if (err)
    return (err);

  memset(buff, 0, sizeof(buff));
  r_len = read(sync->sync_fd, buff, sizeof(buff));
  len = (r_len / sizeof(struct inotify_event));

  for (i = 0; i < len; i++) {
    err = linux_sync_poll_event(u, sync, &event[i]);
    if (err)
      return (err);
  }

  return (0);
}

int linux_sync_remove(fuser_t *u, sync_t *sync, sync_ent_t *ent)
{

  inotify_rm_watch(sync->sync_fd, ent->id);
  return (0);
}


int linux_sync_read(fuser_t *u, sync_t *sync, sync_ent_t *ent)
{
  return (0);
}

int linux_sync_write(fuser_t *u, sync_t *sync, sync_ent_t *ent)
{
  return (0);
}

#endif /* linux */
