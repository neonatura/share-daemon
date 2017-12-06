

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

#ifndef __SYNC__SYNC_LINUX_H__
#define __SYNC__SYNC_LINUX_H__

#ifdef linux
#include <sys/types.h>
#include <dirent.h>
#include <sys/inotify.h>

int linux_sync_init(fuser_t *u, sync_t *scan, void *unused);

int linux_sync_term(fuser_t *u, sync_t *sync, void *unused);

int linux_sync_watch(fuser_t *, sync_t *sync, const char *path);

int linux_sync_poll(fuser_t *u, sync_t *sync, double *to_p);

int linux_sync_remove(fuser_t *u, sync_t *sync, sync_ent_t *ent);

int linux_sync_read(fuser_t *u, sync_t *sync, sync_ent_t *ent);

int linux_sync_write(fuser_t *u, sync_t *sync, sync_ent_t *ent);

#endif


#endif /* ndef __SYNC__SYNC_LINUX_H__ */


