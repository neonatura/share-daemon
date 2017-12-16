
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

#ifndef __FSYNC_H__
#define __FSYNC_H__

#include <sys/types.h>
#include <unistd.h>
#include <utime.h>
#include <dirent.h>
#include <signal.h> 
#include <pwd.h>
#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <share.h>

#define OP_MODIFY 0x2
#define OP_CREATE 0x100
#define OP_DELETE 0x200



/** 
 * A special type indicating no file-system is being monitored.
 */
#define FS_NONE 0
/** 
 * A directory is being monitored on a share-fs parition.
 */
#define FS_SHARE 1
/** 
 * A directory is being monitored hosted on a linux OS.
 */
#define FS_LINUX 2
#define MAX_SYNC_FS 3

/** 
 * A flag indicating a file-system inode entity is being monitored.
 */
#define FSENT_ACTIVE (1 << 0)




/** A file-system specific sync operation. */
typedef int (*sync_f)(void *, void *, void *); /* (fuser_t *, sync_t *, ..) */
#define SYNCF(_f) (sync_f)(_f)

typedef struct sync_op_t
{
  sync_f init;
  sync_f term;
  sync_f watch;
  sync_f poll;
  sync_f remove;
  sync_f read;
  sync_f write;
} sync_op_t;

/**
 * An entity that represents a file-system inode.
 */
typedef struct sync_ent_t
{
  /** The current state of the inode entity. */
  int flags;
  /** A unique reference number for this inode entity. */
  int id;
  /** An absolute path to the inode being watched. */
  char path[PATH_MAX+1];
char hpath[PATH_MAX+1];

  struct stat info;
  shbuf_t *iobuff;

  struct sync_ent_t *next; 
} sync_ent_t;

/**
 * A file-system directory hierarchy.
 */
typedef struct sync_t 
{
  /** The file-system type being monitored. */
  int sync_type;
  int sync_fd;
  /** The root path of the hierarchy being monitored. */
  char sync_path[PATH_MAX+1];

  /** File-system specific operations. */
  sync_op_t *op;

  /** A list of associated inode entities. */
  struct sync_ent_t *ent_list;
} sync_t;



typedef struct fuser_t
{
  char name[256];
  char pass[256];
  char root_path[PATH_MAX+1];
  int err;

  /* shfs */
  shfs_t *fs;
  shkey_t id;

  /* directory hierarchy monitor (local) */
  sync_t lcl_sync;

  /* directory hierarchy monitor (remote) */
  sync_t rem_sync;

  struct pubuser_t *next;
} fuser_t;







#include "fsync_cycle.h"
#include "fsync_pref.h"
#include "fsync_user.h"
#include "fsync_server.h"

#include "sync/sync.h"




#endif /* ndef __FSYNC_USER_H__ */



