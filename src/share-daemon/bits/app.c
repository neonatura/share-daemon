

/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura
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
 *
 *  @endcopyright
 */

#include "sharedaemon.h"

void decr_app_trust(tx_app_t *cli)
{
  shnum_t trust;

  if (!cli)
    return;

  trust = shnum_get(cli->app_trust);
  trust = trust - 1;
  shnum_set(trust, &cli->app_trust);

}

void incr_app_trust(tx_app_t *cli)
{
  shnum_t trust;

  if (!cli)
    return;

  trust = shnum_get(cli->app_trust);
  trust = trust - 1;
  shnum_set(trust, &cli->app_trust);

}

int inittx_app(tx_app_t *app, shpeer_t *peer)
{
  int err;

  memcpy(&app->app_peer, peer, sizeof(shpeer_t));

  err = tx_init(NULL, (tx_t *)app, TX_APP);
  if (err)
    return (err);

  return (0);
}

tx_app_t *alloc_app(shpeer_t *peer)
{
  tx_app_t *app;
  int err;

  app = (tx_app_t *)calloc(1, sizeof(tx_app_t));
  if (!app)
    return (NULL);

  err = inittx_app(app, peer);
  if (err) {
    PRINT_ERROR(err, "alloc_app [initialization]");
    return (NULL);
  } 

  return (app);
}



int txop_app_init(shpeer_t *cli_peer, tx_app_t *app)
{
  shsig_t sig;

//  app->app_arch = app->app_peer.arch;

  app->app_birth = shtime();
  app->app_stamp = shtime();

#if 0
  memset(&sig, 0, sizeof(sig));
  generate_signature(&sig, &app->app_peer, &app->app_tx);
  app->app_birth = sig.sig_stamp;
  memcpy(&app->app_sig, &sig.sig_key, sizeof(shkey_t));
  app->app_stamp = shtime();
#endif


  return (0);
}

int txop_app_confirm(shpeer_t *cli_peer, tx_app_t *app)
{
  shsig_t sig;
  int err;

#if 0
  memset(&sig, 0, sizeof(sig));
  memcpy(&sig.sig_key, &app->app_sig, sizeof(shkey_t));
  sig.sig_stamp = app->app_birth;
  err = confirm_signature(&sig,
      shpeer_kpriv(&app->app_peer), app->app_tx.hash);
  if (err)
    return (err);
#endif

  return (0);
}

int txop_app_recv(shpeer_t *cli_peer, tx_app_t *app)
{
  return (0);
}

int txop_app_send(shpeer_t *cli_peer, tx_app_t *app)
{
  return (0);
}
