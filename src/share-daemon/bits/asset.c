
/*
 * @copyright
 *
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
 *
 *  @endcopyright
 */

#include "sharedaemon.h"

static const char *_asset_labels[MAX_TX_ASSETS] = {
  "none",
  "person",
  "orgranization",
  "system",
  "software",
  "database",
  "network",
  "service",
  "data",
  "device",
  "circuit",
  "daemon",
  "bond"
};

const char *asset_type_label(int type)
{
  type = MAX(0, MIN(MAX_TX_ASSETS - 1, type));
  return (_asset_labels[type]);
}

void generate_asset_signature(tx_asset_t *asset, shpeer_t *peer)
{
  shkey_t *sig_key;
  uint64_t crc;

  if (asset->ass.ass_expire == SHTIME_UNDEFINED)
    asset->ass.ass_expire = shtime_adj(shtime(), SHARE_DEFAULT_EXPIRE_TIME); 

  crc = shcrc((unsigned char *)asset->ass_data, asset->ass_size);
  sig_key = shkey_cert(shpeer_kpriv(peer), crc, asset->ass.ass_expire);
  memcpy(&asset->ass.ass_sig, sig_key, sizeof(shkey_t));
  shkey_free(&sig_key);

}

int verify_asset_signature(tx_asset_t *asset, shpeer_t *peer)
{
  uint64_t crc;
  int err;

  crc = shcrc((unsigned char *)asset->ass_data, asset->ass_size);
  err = shkey_verify(shpeer_kpriv(peer), crc, 
      &asset->ass.ass_sig, asset->ass.ass_expire);
  if (err)
    return (err);

  return (0);
}

/**
 * @note does not return allocated memory 
 */
shpeer_t *load_asset_peer(shkey_t *id_key)
{
  static shpeer_t ret_peer;
  tx_id_t *id;

  id = (tx_id_t *)pstore_load(TX_IDENT, (char *)shkey_hex(id_key));
  if (!id)
    return (NULL);

  memcpy(&ret_peer, &id->id_peer, sizeof(shpeer_t));
  return (&ret_peer);
}

int create_bond_asset(shkey_t *id_key, tx_bond_t *bond, size_t bond_nr, tx_asset_t **asset_p)
{
  tx_asset_t *asset;
  shpeer_t *peer;
  size_t len;
  int err;

  peer = load_asset_peer(id_key);
  if (!peer)
    return (SHERR_INVAL);

  len = sizeof(tx_asset_t) + (sizeof(tx_bond_t) * bond_nr);
  asset = (tx_asset_t *)calloc(len, sizeof(char));
  if (!asset)
    return (SHERR_NOMEM);

  asset->ass.ass_birth = shtime();
  asset->ass_type = TX_BOND;

  /* asset content */
  asset->ass_size = (sizeof(tx_bond_t) * bond_nr);
  memcpy((unsigned char *)asset->ass_data, 
      (unsigned char *)bond, (sizeof(tx_bond_t) * bond_nr));

  /* generate signature based on identity's priveleged key */
  memcpy(&asset->ass.ass_id, id_key, sizeof(asset->ass.ass_id));
  generate_asset_signature(asset, peer);

  err = tx_init(NULL, (tx_t *)asset, TX_ASSET);
  if (err)
    return (err);

  return (0); 
}





tx_asset_t *load_asset(shkey_t *asset_key)
{
  return ((tx_asset_t *)pstore_load(TX_ASSET, (char *)shkey_hex(asset_key)));
}

void save_asset(tx_asset_t *asset)
{
  pstore_save(asset, sizeof(tx_asset_t));
}

void free_asset(tx_asset_t **asset_p)
{
  tx_asset_t *asset;

  if (!asset_p)
    return;

  asset = *asset_p;
  *asset_p = NULL;

  if (!asset)
    return;

  pstore_free(asset);
}

int local_broadcast_asset(tx_asset_t *asset)
{
  sched_tx(asset, sizeof(tx_asset_t) + asset->ass_size);
  return (0);
}

int remote_broadcast_asset(shpeer_t *origin, tx_asset_t *asset)
{
  sched_tx_sink(shpeer_kpriv(origin),  asset, sizeof(tx_asset_t));
  return (0);
}


int confirm_asset(tx_asset_t *asset)
{

/*
  pstore_write(asset->tx.tx_op, 
      (char *)shkey_hex(&asset->ass.ass_sig), raw_data, data_len);
*/

  return (0);
}

int local_confirm_asset(tx_asset_t *asset)
{

  return (0);
}

static int validate_asset_signature(tx_asset_t *asset)
{
  shpeer_t *peer;
  uint64_t crc;
  int err;

  peer = load_asset_peer(&asset->ass.ass_id);
  if (!peer)
    return (SHERR_INVAL);

  crc = shcrc((unsigned char *)asset->ass_data, asset->ass_size);
  err = shkey_verify(shpeer_kpriv(peer), crc, 
      &asset->ass.ass_sig, asset->ass.ass_expire);
  if (err)
    return (err);

  return (0);
}

int remote_validate_asset(tx_app_t *cli, tx_asset_t *asset)
{
  int err;

  /* verify asset signature */
  err = validate_asset_signature(asset);
  if (err) {
//    cli->stinky++;
    return (err);
  }

  return (0);
}

/**
 * A asset notification received from a client on the local machine.
 */
int local_asset_notification(shpeer_t *peer, int tx, unsigned char *asset_data, size_t asset_size)
{

  return (0);
}


/**
 * An incoming asset notification from another server.
 */
int remote_asset_notification(shpeer_t *origin, tx_asset_t *tx)
{
return (0);
}




int process_asset_tx(tx_app_t *cli, tx_asset_t *asset)
{
  tx_asset_t *ent;
  int err;

  err = remote_validate_asset(cli, asset);
  if (err)
    return (err);

  ent = (tx_asset_t *)pstore_load(TX_ASSET, asset->ass_tx.hash);
  if (!ent)
    return (0); /* no state info */

/* .. */

  return (0);
}




int txop_asset_init(shpeer_t *cli_peer, tx_asset_t *asset)
{
  shpeer_t *peer;

  peer = load_asset_peer(&asset->ass.ass_id);
  if (!peer)
    return (SHERR_INVAL);

  generate_asset_signature(asset, peer);

  return (0);
}

int txop_asset_confirm(shpeer_t *cli_peer, tx_asset_t *asset)
{
  shpeer_t *peer;
  uint64_t crc;
  int err;

  peer = load_asset_peer(&asset->ass.ass_id);
  if (!peer)
    return (SHERR_INVAL);

  err = verify_asset_signature(asset, peer);
  if (err)
    return (err);

  return (0);
}


