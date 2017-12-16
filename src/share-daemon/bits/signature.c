
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


#if 0
int confirm_signature(shsig_t *sig, shkey_t *sig_key, char *tx_hash)
{
  uint64_t crc;
  int err;

  crc = (uint64_t)strtoll(tx_hash, NULL, 16);
  err = shkey_verify(&sig->sig_key, crc, sig_key, sig->sig_stamp);
  if (err)
    return (err);

  return (0);
}
void generate_signature(shsig_t *sig, shpeer_t *peer, tx_t *tx)
{
  uint64_t crc;
  shkey_t *sig_key;
  shkey_t *key;
  int err;

  memset(sig, 0, sizeof(shsig_t));

  /* assign origin timestamp */
  sig->sig_stamp = shtime();

  if (!peer)
    peer = ashpeer();

  /* convert hash to checksum */
  crc = (uint64_t)strtoll(tx->hash, NULL, 16);

  sig_key = shkey_cert(shpeer_kpriv(peer), crc, sig->sig_stamp);
  memcpy(&sig->sig_key, sig_key, sizeof(shkey_t));
  shkey_free(&sig_key);


}
#endif






void tx_sign(tx_t *tx, shkey_t *tx_sig, shkey_t *context)
{
  shpeer_t *peer = sharedaemon_peer();
  shkey_t *sig_key;
  shkey_t peer_key;
  uint64_t crc;
  int err;

  memcpy(&peer_key, shpeer_kpriv(peer), sizeof(shkey_t));
  if (tx->tx_stamp == SHTIME_UNDEFINED)
    tx->tx_stamp = shtime();

  sig_key = shkey_cert(context, shkey_crc(&peer_key), tx->tx_stamp);
  memcpy(tx_sig, sig_key, sizeof(shkey_t));
  shkey_free(&sig_key);
}

void tx_sign_context(tx_t *tx, shkey_t *tx_sig, void *data, size_t data_len)
{
  shkey_t *ctx_key;

  ctx_key = shkey_bin(data, data_len);
  tx_sign(tx, tx_sig, ctx_key);
  shkey_free(&ctx_key);
}

int tx_sign_confirm(tx_t *tx, shkey_t *tx_sig, shkey_t *context)
{
  shkey_t sig;
  int err;

  err = shkey_verify(tx_sig, shkey_crc(&tx->tx_peer), context, tx->tx_stamp);
  if (err)
    return (err);

  return (0);
}

void tx_sign_confirm_context(tx_t *tx, shkey_t *tx_sig, void *data, size_t data_len)
{
  shkey_t *ctx_key;

  ctx_key = shkey_bin(data, data_len);
  tx_sign_confirm(tx, tx_sig, ctx_key);
  shkey_free(&ctx_key);
}



