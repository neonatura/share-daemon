
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

shpeer_t *server_peer;

extern int test_main(void);

void txtest_init(void)
{
  server_peer = shapp_init("test", NULL, SHAPP_LOCAL);
}

SHFS *sharedaemon_fs(void)
{
  static SHFS *fs;
  if (!fs) {
    fs = shfs_init(sharedaemon_peer());
  }
  return (fs);
}


int main(int argc, char *argv[])
{
  txtest_init();
  return (txtest_main());
}
