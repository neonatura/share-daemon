
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

#ifndef __SYNC__SCAN_H__
#define __SYNC__SCAN_H__




extern sync_op_t sync_op_table[MAX_SYNC_FS];


/**
 * Monitor a directory hierarchy on a file-system.
 */
int sync_init(fuser_t *user, sync_t *sync, int fs_type, const char *path);

/**
 * Watch a directory in the sync hierarchy.
 */
int sync_watch(fuser_t *user, sync_t *sync, const char *in_path);

/**
 * Stop monitoring a directory in the sync hierarchy.
 */
void sync_remove(fuser_t *user, sync_t *sync, sync_ent_t *ent);

/**
 * Process pending sync operations.
 */
int sync_poll(fuser_t *user, sync_t *sync, double to);

/**
 * De-allocate resources used by sync operations.
 */
int sync_term(fuser_t *user, sync_t *sync);

/**
 * Obtain a sync entity by it's id number.
 */
sync_ent_t *sync_ent_id(sync_t *sync, int ent_id);

/**
 * Obtain a sync entity by it's absolute path reference.
 */
sync_ent_t *sync_ent_path(sync_t *sync, const char *path);

/**
 * Allocate resources associated with a new sync entity.
 */
sync_ent_t *sync_ent_init(sync_t *sync, int id, const char *path);

/**
 * Propagate an inode to all other sync services hosted by a particular user.
 */
int sync_relay_update(fuser_t *user, sync_t *sync, sync_ent_t *ent);


#include "sync_share.h"
#include "sync_linux.h"


#endif /* ndef __SYNC__SCAN_H__ */



