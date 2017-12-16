
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



int inittx_ref(tx_ref_t *ref, tx_t *tx, char *name, char *hash, int type)
{
  shkey_t *tx_key;
  int err;

  memset(ref->ref.ref_name, 0, sizeof(ref->ref.ref_name));
  memset(ref->ref.ref_hash, 0, sizeof(ref->ref.ref_hash));

  /* attributes */
  if (name)
    strncpy(ref->ref.ref_name, name, sizeof(ref->ref.ref_name)-1); 
  if (hash)
    strncpy(ref->ref.ref_hash, hash, sizeof(ref->ref.ref_hash)-1); 
  ref->ref.ref_type = type;
  ref->ref.ref_level = tx->tx_op;

  /* origin peer */
  memcpy(&ref->ref.ref_peer, &tx->tx_peer, sizeof(ref->ref.ref_peer));

  /* populate unique key into 'external hash reference' if none specified. */
  if (!*ref->ref.ref_hash) {
    tx_key = get_tx_key(tx);
    if (tx_key)
      memcpy(&ref->ref.ref_hash, shkey_print(tx_key), sizeof(ref->ref.ref_hash));
  }

  /* generate new reference */
  err = tx_init(NULL, (tx_t *)ref, TX_REFERENCE);
  if (err)
    return (err);

  return (0);
}

int inittx_ref_tx(tx_ref_t *ref, tx_t *tx)
{
  return (inittx_ref(ref, tx, NULL, NULL, TXREF_TX));
}

tx_ref_t *alloc_ref(tx_t *tx, char *name, char *hash, int type)
{
  tx_ref_t *ref;
  int err;

  ref = (tx_ref_t *)calloc(1, sizeof(tx_ref_t));
  if (!ref)
    return (NULL);

  err = inittx_ref(ref, tx, name, hash, type);
  if (err)
    return (NULL);

  return (ref);
}


int txop_ref_init(shpeer_t *cli_peer, tx_ref_t *ref)
{


  return (0);
}

int txop_ref_confirm(shpeer_t *peer, tx_ref_t *ref)
{

  return (0);
}

int txop_ref_send(shpeer_t *peer, tx_ref_t *ref)
{
  return (0);
}

int txop_ref_recv_tx(shpeer_t *peer, tx_ref_t *ref)
{
  tx_ward_t *ward;
  tx_t *tx;
  shkey_t *key;
  shkey_t l_key;
  int err;

  key = shkey_gen(ref->ref.ref_hash);
  memcpy(&l_key, key, sizeof(l_key));
  shkey_free(&key);

  /* load transaction in reference. */
  tx = tx_load(ref->ref.ref_level, &l_key);
  if (!tx)
    return (SHERR_INVAL);

  ward = NULL;
  if (tx->tx_flag & TXF_WARD) {
    /* load ward transaction */
    ward = tx_load(TX_WARD, &l_key);
    if (!ward) {
      err = SHERR_INVAL;
      goto done;
    }

    /* verify context to suppress ward */
    err = txward_context_confirm(ward, ref);
    if (err)
      goto done;

    /* process original transaction. */
    err = tx_recv(peer, tx);
    if (err)
      goto done;
  }

done:
  if (tx) pstore_free(tx);
  if (ward) pstore_free(ward);

  return (0);
}

int txop_ref_recv(shpeer_t *peer, tx_ref_t *ref)
{
  int err;

  switch (ref->ref.ref_type) {
    case TXREF_TX:
      err = txop_ref_recv_tx(peer, ref); 
      if (err)
        return (err);
      break;
  }

  return (0);
}

int txop_ref_wrap(shpeer_t *peer, tx_ref_t *ref)
{
  wrap_bytes(&ref->ref.ref_type, sizeof(ref->ref.ref_type));
  wrap_bytes(&ref->ref.ref_level, sizeof(ref->ref.ref_level));
  return (0);
}



