
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
#include <pwd.h>

#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif


#define PUB_SYNC_PATH "share"

static fuser_t *_sync_user_table;
static int MAX_PUBUSER_NAME_LENGTH; 


static int fsync_user_generate(fuser_t *u)
{
  shkey_t *id_key;
  uint32_t mode;
  int err;

  err = shapp_ident(shpam_uid(u->name), &id_key); 
  if (err)
    return (err);

  /* retain identity name key */
  memcpy(&u->id, id_key, sizeof(shkey_t)); 
  shkey_free(&id_key);

  return (0);
}

fuser_t *fsync_user_add(int uid, char *username, char *userpass, char *path)
{
  fuser_t *u;
  char hostname[256];
  char uname[1024];
  char upass[1024];
  char *ptr;

#ifdef HAVE_GETSPNAM
  /* check for shadow password */
  if (*username) {
    struct spwd *spwd;

    spwd = getspnam(username);
    if (spwd) {
      userpass = spwd->sp_pwdp; /* use shadow passwd */
//fprintf(stderr, "DEBUG: found %s' shadow pass '%s'\n", username, userpass);
}
  }
#endif

  memset(uname, 0, sizeof(uname));
  memset(hostname, 0, sizeof(hostname));
  gethostname(hostname, sizeof(hostname)-1);
  sprintf(uname, "%s@%s", username, hostname);

  memset(upass, 0, sizeof(upass));
  strncpy(upass, userpass, sizeof(upass) - 1);

  fsync_shpref_init(uid);

  ptr = (char *)fsync_shpref_get(SHPREF_ACC_NAME, uname);
  if (0 != strcmp(ptr, uname))
    strncpy(uname, ptr, sizeof(uname));

  ptr = (char *)fsync_shpref_get(SHPREF_ACC_PASS, userpass);
  strncpy(upass, ptr, sizeof(upass));

  fsync_shpref_free();

  /* ensure this is unique path */
  for (u = _sync_user_table; u; u = u->next) {
    if (0 == strcmp(u->root_path, path))
      break;
  }
  if (u)
    return (NULL); /* initial user owns path */

  for (u = _sync_user_table; u; u = u->next) {
    if (0 == strcmp(u->name, uname))
      break;
  }
  if (!u) {
    u = (fuser_t *)calloc(1, sizeof(fuser_t));
    if (!u)
      return (NULL);

    strncpy(u->name, uname, sizeof(u->name) - 1);
  }

  strncpy(u->pass, upass, sizeof(u->pass) - 1);
  strncpy(u->root_path, path, sizeof(u->root_path) - 1);

  fsync_user_generate(u);

  u->next = _sync_user_table;
  _sync_user_table = u;

fprintf(stderr, "DEBUG: fsync_user_add[%x]: uname(%s) upass(%s) path(%s)\n", u, uname, upass, path);

  return (u);
}

void fsync_user_init(void)
{
  struct passwd raw_pw, *pw;
  struct stat st;
  fuser_t *user;
  char path[PATH_MAX+1];
  char buf[4096];
  char uname[4096];
  struct spwd *spwd; 
  uid_t uid;
  int err;

  MAX_PUBUSER_NAME_LENGTH = sysconf(_SC_LOGIN_NAME_MAX);
  if (MAX_PUBUSER_NAME_LENGTH == -1)
    MAX_PUBUSER_NAME_LENGTH = 256;
  MAX_PUBUSER_NAME_LENGTH = MIN(256, MAX_PUBUSER_NAME_LENGTH);


  // setpwent();

  memset(&raw_pw, 0, sizeof(raw_pw));
  memset(buf, 0, sizeof(buf));
  while (0 == 
#if defined(HAVE_GETPWENT_R)
		  getpwent_r(&raw_pw, buf, sizeof(buf), &pw)
#elif defined(HAVE_GETPWENT)
		  (pw = getpwent())
#else
		  TRUE
#endif
		  ) {
    sprintf(path, "%s/%s", pw->pw_dir, PUB_SYNC_PATH);
    err = stat(path, &st);
    if (err)
      continue;
    if (!S_ISDIR(st.st_mode))
      continue;


    user = fsync_user_add(pw->pw_uid, pw->pw_name, pw->pw_passwd, path);
    if (!user) continue;

//fprintf(stderr, "DEBUG: found %s's md5 pass '%s'\n", pw->pw_name, pw->pw_passwd);

#ifdef linux
    /* initialize local fs monitor */
    sync_init(user, &user->lcl_sync, FS_LINUX, user->root_path); 
#endif
  }
  //  endpwent();

}

int fsync_user_validate(fuser_t *u, char *pass)
{
  char *enc;

  if (!u)
    return (SHERR_INVAL);

  if (!pass)
    pass = "";

  if (strlen(u->pass) >= 2) {
    enc = crypt(pass, u->pass);
    if (0 != strcmp(enc, u->pass))
      return (SHERR_ACCESS);
  }

  return (0);
}

void fsync_user_free(void)
{
  fuser_t *u;
  fuser_t *u_next;
  for (u = _sync_user_table; u; u = u_next) {
    u_next = u->next;
    free(u);
  }
  _sync_user_table = NULL;
}

void fsync_user_poll(fuser_t *u)
{
  int err;

err = sync_poll(u, &u->lcl_sync, 0);
if (err) fprintf(stderr, "DEBUG: fsync_user_poll: %d = sync_poll()\n", err);

  
}

void fsync_user_scan(void)
{
  fuser_t *u;

  for (u = _sync_user_table; u; u = u->next) {
    fsync_user_poll(u);
  }

}


