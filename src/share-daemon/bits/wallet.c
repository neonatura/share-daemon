
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


int inittx_wallet_channel(tx_wallet_t *wal, shkey_t *origin, shkey_t *peer, shkey_t *redeem)
{
  int err;

  memset(wal, 0, sizeof(tx_wallet_t));
  wal->wal_type = TXWALLET_CHANNEL;

  memset(wal->wal_cur, 0, sizeof(wal->wal_cur));
  strncpy(wal->wal_cur, COIN_SHC, sizeof(wal->wal_cur)-1); 

  memcpy(&wal->wal_origin, origin, sizeof(shkey_t));
  memcpy(&wal->wal_peer, peer, sizeof(shkey_t));
  memcpy(&wal->wal_redeem, redeem, sizeof(shkey_t));

  err = tx_init(NULL, (tx_t *)wal, TX_WALLET);
  if (err)
    return (err);

  return (0);
}

int inittx_wallet(tx_wallet_t *wal, char *type, char *name, char *key)
{
  int err;

  if (!type || !name || !key)
    return (SHERR_INVAL);

  memset(wal->wal_cur, 0, sizeof(wal->wal_cur));
  memset(wal->wal_name, 0, sizeof(wal->wal_name));
  memset(wal->wal_key, 0, sizeof(wal->wal_key));

  strncpy(wal->wal_cur, type, sizeof(wal->wal_cur)-1); 
  strncpy(wal->wal_name, name, sizeof(wal->wal_name)-1); 
  strncpy(wal->wal_key, key, sizeof(wal->wal_key)-1); 

  err = tx_init(NULL, (tx_t *)wal, TX_WALLET);
  if (err)
    return (err);

  return (0);
}

tx_wallet_t *alloc_wallet(char *type, char *name, char *key)
{
  tx_wallet_t *wallet;
  int err;

  wallet = (tx_wallet_t *)calloc(1, sizeof(tx_wallet_t));
  if (!wallet)
    return (NULL);

  err = inittx_wallet(wallet, type, name, key);
  if (err)
    return (NULL);

  return (wallet);
}


int txop_wallet_init(shpeer_t *cli_peer, tx_wallet_t *wallet)
{
  shkey_t *key;

  wallet->wal_birth = shtime();

  key = shkey_hexgen(wallet->wal_key + 8);
  if (!key)
    return (SHERR_NOKEY);
  memcpy(&wallet->wal_sig, key, sizeof(wallet->wal_sig));
  shkey_free(&key);

  wallet->wal_stamp = shtime();

  return (0);
}

int txop_wallet_confirm(shpeer_t *peer, tx_wallet_t *wallet)
{
  shtime_t now;
  shkey_t *key;
  int sig_ok;

  if (0 != strcmp(wallet->wal_cur, COIN_USDE) &&
      0 != strcmp(wallet->wal_cur, COIN_EMC2) &&
      0 != strcmp(wallet->wal_cur, COIN_SHC))
    return (SHERR_INVAL);

  now = shtime();
  if (shtime_before(shtime(), wallet->wal_birth) ||
      shtime_before(shtime(), wallet->wal_stamp) ||
      shtime_after(wallet->wal_birth, wallet->wal_stamp))
    return (SHERR_TIME);

  key = shkey_hexgen(wallet->wal_key + 8);
  if (!key)
    return (SHERR_NOKEY);
  sig_ok = shkey_cmp(key, &wallet->wal_sig);
  shkey_free(&key);
  if (!sig_ok)
    return (SHERR_KEYREJECTED);

  return (0);
}

int txop_wallet_send(shpeer_t *peer, tx_wallet_t *wallet)
{
  return (0);
}

int txop_wallet_recv(shpeer_t *peer, tx_wallet_t *wallet)
{
  //wallet->wal_stamp = shtime();
  return (0);
}

int txop_wallet_wrap(shpeer_t *cli_peer, tx_wallet_t *wal)
{
  wrap_bytes(&wal->wal_type, sizeof(wal->wal_type));
  return (0);
}


