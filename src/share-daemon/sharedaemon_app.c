

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

#include "sharedaemon.h"


/**
 * initializes a libshare runtime enabled process application
 */
int sharedaemon_app_init(shd_t *cli, shpeer_t *peer)
{
  tx_t tx;
  tx_id_t id;
  shkey_t *app_key;
  shsig_t sig;
  tx_app_t *app;
  tx_init_t ini;
  int flags = cli->flags;
  int err;

fprintf(stderr, "DEBUG: sharedaemon_app_init: peer '%s'\n", shpeer_print(peer));

  memcpy(&cli->peer, peer, sizeof(shpeer_t));

  err = inittx_app(&cli->app, peer);
  if (err)
    return (err);

  if (!(flags & SHD_CLIENT_AUTH) && !(flags & SHD_CLIENT_SHUTDOWN)) {
    if (!(cli->flags |= SHD_CLIENT_REGISTER)) {
      /* broadcast app initializeation */
      err = tx_send(NULL, &cli->app);
      if (err) {
        PRINT_ERROR(err, "sharedaemon_app_init [tx_send]");
      }

      cli->flags |= SHD_CLIENT_REGISTER;
    }
  }

  return (0);
}


