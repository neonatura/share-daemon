
/*
 *  Copyright 2015 Neo Natura 
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

int peer_fresh(tx_app_t *app)
{
  int dur;

  dur = 86400 / (app->app_trust+1);
  if (shtime_after(shtime_adj(app->app_stamp, dur), shtime())) {
    /* too soon */
    return (TRUE);
  }

  return (FALSE);
}

int peer_add(shpeer_t *peer)
{
  tx_app_t *app; 
  int err;

  if (!peer)
    return (0);
  if (!*peer->label || 0 == strcmp(peer->label, PACKAGE))
    return (0);

  if (shpeer_localhost(peer)) /* share.c */
    return (0); /* all done */

  /* private key is lookup field on TX_APP transaction */
  app = pstore_load(TX_APP, (char *)shkey_hex(shpeer_kpriv(peer)));
  if (app && peer_fresh(app))
    return (0);

  err = peer_verify(peer);
fprintf(stderr, "DEBUG: PEER_ADD: VERIFY: %d = peer_verify()\n", err);
  if (err)
    return (err);

  return (0);
}

int peer_verify(shpeer_t *peer)
{
#if 0
  tx_app_t *app = *app_p;
#endif
  int err;
  int sk;

  if (sharedaemon_client_find(shpeer_kpriv(peer)))
    return (0); /* op already in progress */

  sk = SHERR_OPNOTSUPP;
fprintf(stderr, "DEBUG: peer->addr.sin_family %d\n", (int)peer->addr.sin_family);
  switch (peer->addr.sin_family) {
    case AF_INET:
    case AF_INET6:
      /* create a new socket connection to test peer's validity. */
      sk = shconnect_peer(peer, SHNET_CONNECT | SHNET_SHUTDOWN | SHNET_TRACK);
fprintf(stderr, "DEBUG: %d = shconnect_peer(): %s\n", sk, shpeer_print(peer));
      break;
  }
  if (sk < 0) {
    /* immediate error causes transaction failure. */
    sherr(sk, shpeer_print(peer));
    return (sk);
  }
 
  err = sharedaemon_netclient_add(sk, peer, SHD_CLIENT_SHUTDOWN);
  if (err) {
    close(sk);
    return (err);
  }

#if 0
  /* network engine will incr/decr app's "app_trust" based on connect attempt */
  if (!app)
    app = init_app(peer);
  client_queue_add(sk, SHNET_VERIFY, app);
  *app_p = app;
#endif

  return (0);
}


