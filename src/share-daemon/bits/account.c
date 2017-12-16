

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
tx_account_t *sharedaemon_account(void)
{
  return (alloc_account(shpam_uid("root"), shapp_root()));
}
#endif

int inittx_account(tx_account_t *acc, uint64_t uid)
{
  shseed_t seed;
  shauth_t *auth;
  shfs_t *fs;
  shfs_ino_t *file;
  int err;

  acc->acc_uid = uid;

  fs = NULL;
  file = shpam_shadow_file(&fs);
  if (!file)
    return (SHERR_IO);

  /* record peer */
  memcpy(&acc->acc_peer, shfs_peer(fs), sizeof(acc->acc_peer));

  err = shpam_shadow_auth_load(file, uid, SHAUTH_SCOPE_REMOTE, &acc->acc_auth);
  shfs_free(&fs);
  if (err)
    return (err);  

  err = tx_init(NULL, (tx_t *)acc, TX_ACCOUNT);
  if (err) { 
    PRINT_ERROR(err, "inittx_account [initialization error]");
    return (err);
  }

  return (0);
}

tx_account_t *alloc_account(uint64_t uid)
{
	tx_account_t *acc;
  int err;

  acc = (tx_account_t *)calloc(1, sizeof(tx_account_t));
  if (!acc)
    return (NULL);

  err = inittx_account(acc, uid);
  if (err) {
    PRINT_ERROR(err, "alloc_account [initialization]");
    free(acc);
    return (NULL);
  }

  return (acc);
}

#if 0
tx_account_t *alloc_account_user(char *username, char *passphrase, uint64_t salt)
{
  tx_account_t *acc;

  acc = alloc_account(shpam_pass_gen(username, passphrase, salt));
  if (!acc)
    return (NULL);

  return (acc);
}
#endif



int txop_account_init(shpeer_t *cli_peer, tx_account_t *acc)
{
  return (0);
}

int txop_account_confirm(shpeer_t *cli_peer, tx_account_t *acc)
{

  /* verify parameters */
  if (acc->acc_auth.auth_salt == 0)
    return (SHERR_INVAL);
  if (acc->acc_auth.auth_stamp == SHTIME_UNDEFINED)
    return (SHERR_INVAL);
  if (shalg_size(acc->acc_auth.auth_pub) == 0)
    return (SHERR_INVAL);
  if (shalg_size(acc->acc_auth.auth_sig) == 0)
    return (SHERR_INVAL);

  return (0);
}

int txop_account_send(shpeer_t *cli_peer, tx_account_t *acc)
{

  return (0);
}

int txop_account_recv(shpeer_t *cli_peer, tx_account_t *acc)
{
  shseed_t seed;
  shauth_t *auth;
  shfs_t *fs;
  SHFL *shadow_file;
  uint64_t uid;
  int err;

  if (!acc)
    return (SHERR_INVAL);

  uid = acc->acc_uid;
  auth = &acc->acc_auth;



  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);

  err = shpam_shadow_remote_set(shadow_file, uid, auth);
  shfs_free(&fs);
  if (err)
    return (err);

#if 0
  /* verify uid (TX_IDENT) already exists. */

  err = shpam_shadow_load(shadow_file, uid, &seed);
  if (err) {
    shfs_free(&fs);
    return (err);
  }

  err = shpam_auth_remote_set(&seed, uid, auth);
  if (err) {
    if (err == SHERR_ACCESS)
      return (0); /* soft error */
    return (err);
  }

  /* attempt to add to local pam db */
  err = shpam_pshadow_store(shadow_file, &seed);
  shfs_free(&fs);
  if (err)
    return (err);
#endif

  return (0);
}

int txop_account_wrap(shpeer_t *peer, tx_account_t *acc)
{
  wrap_bytes(&acc->acc_auth.auth_alg, sizeof(acc->acc_auth.auth_alg));
  wrap_bytes(&acc->acc_auth.auth_flag, sizeof(acc->acc_auth.auth_flag));
  wrap_bytes(acc->acc_auth.auth_pub, sizeof(acc->acc_auth.auth_pub));
  wrap_bytes(acc->acc_auth.auth_sig, sizeof(acc->acc_auth.auth_sig));
}
