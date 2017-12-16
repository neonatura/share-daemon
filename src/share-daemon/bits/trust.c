
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

#include "bits.h"
#include "../sharedaemon_file.h"




int inittx_trust(tx_trust_t *trust, tx_t *ref_tx, shkey_t *context)
{
  shkey_t *tx_key;
  int err;

  if (!trust)
    return (SHERR_INVAL);

  tx_key = get_tx_key(ref_tx);
  if (!tx_key)
    return (SHERR_INVAL);

  if (!context)
    context = ashkey_blank();

  memcpy(&trust->trust_ref, tx_key, sizeof(shkey_t));
  memcpy(&trust->trust_ctx, context, sizeof(shkey_t));

  err = tx_init(NULL, trust, TX_TRUST);
  if (err)
    return (err);

  return (0);
}




int txop_trust_init(shpeer_t *cli_peer, tx_trust_t *trust)
{
  shkey_t *sig_key;
  uint64_t crc;
  int err;

  if (!trust)
    return (SHERR_INVAL);

  if (shkey_cmp(&trust->trust_ctx, ashkey_blank()))
    memcpy(&trust->trust_ctx, ashkey_uniq(), sizeof(trust->trust_ctx));
  tx_sign(trust, &trust->trust_sig, &trust->trust_ctx);

  return (0);
}

int txop_trust_confirm(shpeer_t *cli_peer, tx_trust_t *trust)
{
  uint64_t crc;
  int err;

  err = tx_sign_confirm(trust, &trust->trust_sig, &trust->trust_ctx);
  if (err)
    return (err);

  return (err);
}


int txop_trust_recv(shpeer_t *cli_peer, tx_trust_t *trust)
{
  int err;

#if 0
  incr_app_trust(cli);
#endif  

  return (0);
}
