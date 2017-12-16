
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

shpeer_t *_fsync_peer;

static void fsync_free(void)
{
  fsync_cycle_free();
  shpeer_free(&_fsync_peer);
}

void fsync_terminate(int sig_num)
{

  signal(sig_num, SIG_DFL);

  fsync_free();

  raise(sig_num);

}

void fsync_signal(void)
{
  signal(SIGTERM, fsync_terminate);
}

int main(int argc, char *argv[])
{

  _fsync_peer = shapp_init(argv[0], NULL, 0);
#if 0
{
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  struct spwd *spwd;
  pubuser_t *u;
  int err;

  spwd = getspnam(pw->pw_name);
  if (spwd)
    pw->pw_passwd = spwd->sp_pwdp; /* use shadow passwd */

  fprintf(stderr, "DEBUG: user '%s'\n"
      "\tpasswd: %s\n"
      "\tuid: %d\n"
      "\tgid: %d\n"
      "\tgecos: %s\n"
      "\tdir: %s\n"
      "\tshell: %s\n",
      pw->pw_name,
      pw->pw_passwd,
      pw->pw_uid,
      pw->pw_gid,
      pw->pw_gecos,
      pw->pw_dir,
      pw->pw_shell);

  u = fsync_user_add(pw->pw_name, pw->pw_passwd, pw->pw_dir);

  err = fsync_user_validate(u, "..");
fprintf(stderr, "DEBUG: %d = fsync_user_validate(%s, ..)\n", err, u->name);
exit(1);
}
#endif

  //daemon(0, 1);

  fsync_signal();

  fsync_cycle();

  shpeer_free(&_fsync_peer);
 
  return (0);
}


