
/*
 * @copyright
 *
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
 *
 *  @endcopyright
 */
#include "sharedaemon.h"

int inittx_subscribe(tx_subscribe_t *sub, shkey_t *key, int op_type, int flags)
{
  int err;

#if 0
  memcpy(&sub->sub_peer, peer, sizeof(sub->sub_peer));
#endif
  memcpy(&sub->sub_key, key, sizeof(sub->sub_key));
  sub->sub_op = op_type;
  sub->sub_flag = flags;

  err = tx_init(NULL, (tx_t *)sub, TX_SUBSCRIBE);
  if (err)
    return (err);

  return (0);
}

tx_subscribe_t *alloc_subscribe(shkey_t *key, int op_type, int flags)
{
  tx_subscribe_t *sub;
  int err;

  sub = (tx_subscribe_t *)calloc(1, sizeof(tx_subscribe_t));
  if (!sub)
    return (NULL);

  err = inittx_subscribe(sub, key, op_type, flags);
  if (err) {
    free(sub);
    return (NULL);
  }

  return (sub);
}


int txop_sub_init(shpeer_t *cli_peer, tx_subscribe_t *sub)
{
  shpeer_t *peer = sharedaemon_peer();

#if 0
  memcpy(&sub->sub_peer, peer, sizeof(sub->sub_peer));
#endif

  return (0);
}

int txop_sub_confirm(shpeer_t *cli_peer, tx_subscribe_t *sub)
{

  if (sub->sub_tx.tx_op >= MAX_VERSION_TX)
    return (SHERR_INVAL);

  if (sub->sub_tx.tx_op >= MAX_TX)
    return (SHERR_OPNOTSUPP);

  return (0);
}

int txop_sub_recv_listen(shpeer_t *cli_peer, tx_subscribe_t *sub)
{
  shd_t *cli;
  int err;

  if (!cli)
    return (SHERR_NOENT);

  cli = sharedaemon_client_find(shpeer_kpriv(cli_peer));
  if (!cli)
    return (SHERR_NOENT);

  err = sharedaemon_client_listen(cli, sub);
  if (err)
    return (err);

  return (0);
}
int txop_sub_recv(shpeer_t *cli_peer, tx_subscribe_t *sub)
{

  if (sub->sub_flag & SHOP_LISTEN) {
    txop_sub_recv_listen(cli_peer, sub);
  }

  return (0);
}
int txop_sub_wrap(shpeer_t *cli_peer, tx_subscribe_t *sub)
{
  wrap_bytes(&sub->sub_op, sizeof(sub->sub_op));
  wrap_bytes(&sub->sub_flag, sizeof(sub->sub_flag));
}
