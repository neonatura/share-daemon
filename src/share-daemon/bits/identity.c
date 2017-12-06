
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


#if 0
static int local_ident_shadow_generate(tx_id_t *id)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  int err;

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);

  err = shpam_shadow_load(shadow_file, id->id_uid, NULL);
  if (err == SHERR_NOKEY || err == SHERR_NOENT) {
    err = shpam_shadow_create(shadow_file, id->id_uid, NULL); 
  }

  shfs_free(&fs);

  return (err);
}
#endif

#if 0
static int local_ident_shadow_verify(tx_id_t *id)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  int err;

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  err = shpam_shadow_load(shadow_file, id->id_uid, NULL);
  shfs_free(&fs);

  return (err);
}
#endif

#if 0
int local_ident_generate(uint64_t uid, shpeer_t *app_peer, tx_id_t **id_p)
{
  tx_id_t *l_id;
  tx_id_t *id;
  shkey_t *key;
  int err;

  /* lookup id key in case of existing record */
  key = shpam_ident_gen(uid, app_peer);
  l_id = (tx_id_t *)pstore_load(TX_IDENT, (char *)shkey_hex(key));
  if (l_id) {
    shkey_free(&key);

    err = global_ident_confirm(l_id);
    if (err)
      return (err);

    *id_p = l_id;
    return (0);
  }

  id = (tx_id_t *)calloc(1, sizeof(tx_id_t));
  if (!id) {
    shkey_free(&key);
    return (SHERR_NOMEM);
  }

  memset(id, 0, sizeof(tx_id_t));
  if (app_peer)
    memcpy(&id->id_peer, app_peer, sizeof(shpeer_t));
  id->id_uid = uid;
  memcpy(&id->id_key, key, sizeof(shkey_t));
  shkey_free(&key);

  err = local_ident_shadow_generate(id);
  if (err)
    return (err);

  local_transid_generate(TX_IDENT, &id->id_tx);

  err = remote_ident_inform(id);
  if (err)
    return (err);

  pstore_save(id, sizeof(tx_id_t));

  *id_p = id;
  return (0);
}
int local_ident_inform(tx_app_t *cli, tx_id_t *id)
{
  tx_id_t *ent;
  int err;

  ent = (tx_id_t *)pstore_load(TX_IDENT, id->id_tx.hash);
  if (!ent) {
    /* broadcast to relevant peers */
    err = remote_ident_inform(id);
    if (err)
      return (err);

    pstore_save(id, sizeof(tx_id_t));
  }

  return (0);
}
#endif

int txop_ident_recv(shpeer_t *cli_peer, tx_id_t *id)
{
  return (0);
}
int txop_ident_send(shpeer_t *cli_peer, tx_id_t *id, tx_id_t *ent)
{
  return (0);
}
int txop_ident_init(shpeer_t *cli_peer, tx_id_t *id)
{
  shkey_t *key;
  int err;

  key = shpam_ident_gen(id->id_uid, &id->id_peer);
  if (!key)
    return (SHERR_INVAL);

  memcpy(&id->id_key, key, sizeof(shkey_t));
  shkey_free(&key);

#if 0
  err = local_ident_shadow_generate(id);
  if (err)
    return (err);
#endif

  return (0);
}

int txop_ident_confirm(shpeer_t *cli_peer, tx_id_t *id, tx_id_t *ent)
{
  shkey_t *key;
  int err;

  /* ensure id key is derived from ident's base attributes */
  err = shpam_ident_verify(&id->id_key, id->id_uid, &id->id_peer);
  if (err)
    return (err);

#if 0
  err = local_ident_shadow_verify(id);
  if (err)
    return (err);
#endif

  return (0);
}




int inittx_ident(tx_id_t *id, uint64_t uid, shpeer_t *app_peer)
{
  tx_id_t *l_id;
  shkey_t *key;
  int err;

  /* lookup id key in case of existing record */
  key = shpam_ident_gen(uid, app_peer);
#if 0
  l_id = (tx_id_t *)pstore_load(TX_IDENT, (char *)shkey_hex(key));
  shkey_free(&key);
  if (l_id) {
    memcpy(id, l_id, sizeof(tx_id_t));
    pstore_free(l_id);
    return (0);
  }
#endif

  if (app_peer)
    memcpy(&id->id_peer, app_peer, sizeof(shpeer_t));
  id->id_uid = uid;

  err = tx_init(app_peer, (tx_t *)id, TX_IDENT);
  if (err)
    return (err);

  return (0);
}

tx_id_t *alloc_ident(uint64_t uid, shpeer_t *app_peer)
{
  tx_id_t *id;
  int err;

  id = (tx_id_t *)calloc(1, sizeof(tx_id_t));
  if (!id)
    return (NULL);

  err = inittx_ident(id, uid, app_peer);
  if (err) {
    free(id);
    return (NULL);
  }

  return (id);
}


