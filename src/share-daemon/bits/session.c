

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


int inittx_session(tx_session_t *sess, uint64_t uid, shkey_t *id_key, shtime_t stamp)
{
  int err;

  sess->sess_stamp = stamp;
  sess->sess_uid = uid;

  if (id_key) {
    memcpy(&sess->sess_id, id_key, sizeof(sess->sess_id));
  } else {
    id_key = shpam_ident_gen(uid, ashpeer());
    memcpy(&sess->sess_id, id_key, sizeof(sess->sess_id));
    shkey_free(&id_key);
  }

  err = tx_init(NULL, (tx_t *)sess, TX_SESSION);
  if (err)
    return (err);

  return (0);
}

tx_session_t *alloc_session(uint64_t uid, shkey_t *id_key, shtime_t stamp)
{
  tx_session_t *sess;
  int err;

  sess = (tx_session_t *)calloc(1, sizeof(tx_session_t));
  if (!sess)
    return (NULL);

  err = inittx_session(sess, uid, id_key, stamp);
  if (err) {
    PRINT_ERROR(err, "alloc_session [initialize]");
    return (NULL);
  }

  return (sess); 
}

tx_session_t *alloc_session_peer(uint64_t uid, shpeer_t *peer)
{
  tx_session_t *sess;
  shkey_t *key;
  uint64_t stamp;

  stamp = shtime_adj(shtime(), SHARE_DEFAULT_EXPIRE_TIME);

  key = shpam_ident_gen(uid, peer);
  sess = alloc_session(uid, key, stamp);
  shkey_free(&key);

  return (sess);
}





int txop_session_init(shpeer_t *cli_peer, tx_session_t *sess)
{
  shfs_t *fs;
  SHFL *shadow_file;
  shseed_t seed;
  shkey_t seed_key;
  shkey_t *key;
  char buf[256];
  int err;

  if (sess->sess_stamp == SHTIME_UNDEFINED)
    sess->sess_stamp = shtime_adj(shtime(), MAX_SHARE_SESSION_TIME);

#if 0
  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);

  memset(&seed, 0, sizeof(seed));
  err = shpam_pshadow_load(shadow_file, sess->sess_uid, &seed);
  if (err) {
    shfs_free(&fs);
    return (err);
  }

  shpam_master_key(&seed, &seed_key);
  key = shpam_sess_gen(&seed_key, sess->sess_stamp, &sess->sess_id);
  if (!key) {
    shfs_free(&fs);
    return (SHERR_INVAL);
  }
  memcpy(&sess->sess_key, key, sizeof(sess->sess_key));
  shkey_free(&key);

  shfs_free(&fs);
#endif

  return (0);
}

int txop_session_confirm(shpeer_t *cli_peer, tx_session_t *sess)
{
  shseed_t seed;
  shkey_t seed_key;
  int err;

  if (!sess)
    return (SHERR_INVAL);

  if (shtime_after(shtime(), sess->sess_stamp))
    return (SHERR_KEYEXPIRED);

#if 0
  err = shapp_account_info(sess->sess_uid, NULL, &seed);
  if (err)
    return (err);

  shpam_master_key(&seed, &seed_key);
  err = shpam_sess_verify(&sess->sess_key,
      &seed_key, sess->sess_stamp, &sess->sess_id);
  if (err)
    return (err);
#endif

  return (0);
}

int txop_session_recv(shpeer_t *cli_peer, tx_session_t *sess)
{
  shfs_t *fs;
  SHFL *shadow_file;
  shadow_t shadow;
  shseed_t seed;
  int err;

  if (!sess)
    return (SHERR_INVAL);

#if 0
  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);

  err = shpam_pshadow_load(shadow_file, sess->sess_uid, &seed);
  if (err) {
    shfs_free(&fs);
    return (err);
  }

  err = shpam_shadow_load(shadow_file, sess->sess_uid, &shadow);
  if (err) {
    shfs_free(&fs);
    return (err);
  }

  if (shkey_cmp(&sess->sess_id, &shadow.sh_id)) {
    /* over-write local instance to reflect new session token */
    err = shpam_shadow_session_set(shadow_file, sess->sess_uid, 
        &sess->sess_id, sess->sess_stamp, &sess->sess_key);
    if (err) {
      shfs_free(&fs);
      return (err);
    }
  }

  shfs_free(&fs);
#endif

  return (0);
}

int txop_session_send(shpeer_t *cli_peer, tx_session_t *sess)
{
  return (0);
}



