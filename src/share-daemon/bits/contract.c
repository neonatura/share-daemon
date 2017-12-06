
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



int inittx_contract(tx_contract_t *con, char *type, char *name, char *key)
{
  int err;

  if (!type || !name || !key)
    return (SHERR_INVAL);

  memset(con->con_cur, 0, sizeof(con->con_cur));
  memset(con->con_name, 0, sizeof(con->con_name));
  memset(con->con_key, 0, sizeof(con->con_key));

  strncpy(con->con_cur, type, sizeof(con->con_cur)-1); 
  strncpy(con->con_name, name, sizeof(con->con_name)-1); 
  strncpy(con->con_key, key, sizeof(con->con_key)-1); 

  err = tx_init(NULL, (tx_t *)con, TX_WALLET);
  if (err)
    return (err);

  return (0);
}

tx_contract_t *alloc_contract(char *type, char *name, char *key)
{
  tx_contract_t *contract;
  int err;

  contract = (tx_contract_t *)calloc(1, sizeof(tx_contract_t));
  if (!contract)
    return (NULL);

  err = inittx_contract(contract, type, name, key);
  if (err)
    return (NULL);

  return (contract);
}


int txop_contract_init(shpeer_t *cli_peer, tx_contract_t *contract)
{
  shkey_t *key;

  contract->con_birth = shtime();

  key = shkey_hexgen(contract->con_key + 8);
  if (!key)
    return (SHERR_NOKEY);
  memcpy(&contract->con_sig, key, sizeof(contract->con_sig));
  shkey_free(&key);

  contract->con_stamp = shtime();

  return (0);
}

int txop_contract_confirm(shpeer_t *peer, tx_contract_t *contract)
{
  shtime_t now;
  shkey_t *key;
  int sig_ok;

  if (0 != strcmp(contract->con_cur, COIN_USDE) &&
      0 != strcmp(contract->con_cur, COIN_GMC) &&
      0 != strcmp(contract->con_cur, COIN_SYS))
    return (SHERR_INVAL);

  now = shtime();
  if (shtime_before(shtime(), contract->con_birth) ||
      shtime_before(shtime(), contract->con_stamp) ||
      shtime_after(contract->con_birth, contract->con_stamp))
    return (SHERR_TIME);

  key = shkey_hexgen(contract->con_key + 8);
  if (!key)
    return (SHERR_NOKEY);
  sig_ok = shkey_cmp(key, &contract->con_sig);
  shkey_free(&key);
  if (!sig_ok)
    return (SHERR_KEYREJECTED);

  return (0);
}

int txop_contract_send(shpeer_t *peer, tx_contract_t *contract)
{
  return (0);
}

int txop_contract_recv(shpeer_t *peer, tx_contract_t *contract)
{
  //contract->con_stamp = shtime();
  return (0);
}



