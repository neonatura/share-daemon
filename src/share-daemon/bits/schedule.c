
/*
 * @copyright
 *
 *  Copyright 2013, 2014 Neo Natura
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

//static sh_task_t schedule[MAX_SCHEDULE_TASKS];

void sched_tx_payload(shkey_t *dest_key, void *data, size_t data_len, void *payload, size_t payload_len)
{
  tx_t *tx = (tx_t *)data;
  tx_t sig_tx;
  tx_net_t net;
  shsig_t sig;
  shsig_t new_sig;
  shbuf_t *buff;
  shpeer_t *self_peer;

  if (!data)
    return;
  if (data_len < sizeof(tx_t))
    return;

  prep_net_tx(tx, &net, dest_key, data_len + payload_len); 

  buff = shbuf_init();
  shbuf_cat(buff, &net, sizeof(tx_net_t));
  shbuf_cat(buff, data, data_len);
  if (payload && payload_len)
    shbuf_cat(buff, payload, payload_len);
tx_wrap(NULL, shbuf_data(buff) + sizeof(tx_net_t));
  broadcast_raw(shbuf_data(buff), shbuf_size(buff));
  shbuf_free(&buff);

fprintf(stderr, "DEBUG: SEND: sched_tx_payload: [OP %d] hash(%s) peer(%s) stamp(%llu) nonce(%d) method(%d) OP(%d)\n", (int)tx->tx_op, tx->hash, shkey_print(&tx->tx_peer), (unsigned long long)tx->tx_stamp, (int)tx->nonce, TXHASH_SCRYPT, (int)tx->tx_op);
}

void sched_tx(void *data, size_t data_len)
{
  sched_tx_payload(NULL, data, data_len, NULL, NULL);
}

void sched_tx_sink(shkey_t *dest_key, void *data, size_t data_len)
{
  sched_tx_payload(dest_key, data, data_len, NULL, NULL);
}


