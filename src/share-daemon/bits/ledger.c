
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

#define LEDGERF_UPDATE (1 << 0)

/** @todo don't need to send around payload after initial proposal. */
void propose_ledger(tx_ledger_t *led, tx_t *payload, size_t size)
{

#if 0
  led->ledger_confirm++;
  led->ledger_height = size;
  save_ledger(led, payload, "pending");

  /* ship 'er off */
  generate_transaction_id(TX_LEDGER, &led->tx, NULL);
  sched_tx_payload(led, sizeof(tx_ledger_t), 
      (char *)(led + sizeof(tx_ledger_t)),
      (led->ledger_height * sizeof(tx_t)));
#endif

}

void abandon_ledger(tx_t *tx)
{
}

#if 0
int confirm_ledger(tx_ledger_t *led, tx_t *payload)
{
  tx_ledger_t *p_led = NULL;
  tx_t *ptx_led = NULL;
  tx_ledger_t *t_led;
  tx_t *ttx_led = NULL;
  int bcast;
  int err;

  if (led->ledger_seq != 0) {
    /* find parent ledger */
    err = load_ledger(led->parent_hash, "confirm", &p_led, &ptx_led);
    if (err)
      return (err);
  }

  if (led->ledger_seq != p_led->ledger_seq + 1)
    return (SHERR_ILSEQ);

  err = load_ledger(led->ledger_tx.hash, "confirm", &t_led, &ttx_led);
  if (!err) {
    free_ledger(&p_led, &ptx_led);
    free_ledger(&t_led, &ttx_led);
    return (err); /* this ledger entry has already been stored. */
  }

  bcast = FALSE;

  err = load_ledger(led->ledger_tx.hash, "pending", &t_led, &ttx_led);
  if (err) {
    /* new ledger entry. */
    bcast = TRUE;
    free_ledger(&t_led, &ttx_led);
  } 

  if (led->ledger_confirm >= led->ledger_height) {
    /* broadcast on initial confirmation inform. */
    led->ledger_stamp = shtime(); 
    bcast = TRUE;
    if (led->ledger_stamp == 0) {
      /* this ledger is now confirmed -- inform peers. */
      led->ledger_stamp = shtime(); 
      remove_ledger(led->ledger_tx.hash, "pending"); 
    }
    bcast = TRUE;
    save_ledger(led, payload, "confirm");
  } else {
    save_ledger(led, payload, "pending");
  }

#if 0
  if (bcast) {
    sched_tx_payload(led, sizeof(tx_ledger_t), 
        payload, (led->ledger_height * sizeof(tx_t)));
  }
#endif

  if (led->ledger_confirm >= led->ledger_height &&
      led->ledger_stamp == 0) {
    led->ledger_stamp = shtime(); 
  }

  return (0);
}
#endif



#if 0
int load_ledger(char *hash, char *type, tx_ledger_t **ledger_p, tx_t **payload_p)
{
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  sprintf(path, "/shnet/ledger/%s/%s", type, hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_read(fl, &data, &data_len);
  if (err)
    return (err);
  if (data_len != sizeof(tx_ledger_t)) {
    PRINT_ERROR(SHERR_IO, "[load_ledger] invalid file size");
    return (SHERR_IO);
  }
  *ledger_p = (tx_ledger_t *)data;

  sprintf(path, "/shnet/ledger/%s/%s.tx", type, hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_read(fl, &data, &data_len);
  if (err)
    return (err);
  if (data_len != (sizeof(tx_t) * (*ledger_p)->ledger_height)) {
    PRINT_ERROR(SHERR_IO, "[load_ledger] invalid file size");
    return (SHERR_IO);
  }
  *payload_p = (tx_t *)data;

  return (0);
}
#endif


#if 0
int save_ledger(tx_ledger_t *ledger, tx_t *payload, char *type)
{
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  int err;

  sprintf(path, "/shnet/ledger/%s/%s", type, ledger->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, (char *)ledger, sizeof(tx_ledger_t));
  if (err)
    return (err);

  sprintf(path, "/shnet/ledger/%s/%s.tx", type, ledger->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, (char *)payload,
      (sizeof(tx_t) * ledger->ledger_height));
  if (err)
    return (err);

  return (0);
}
#endif

#if 0
int remove_ledger(tx_ledger_t *ledger, char *type)
{
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  int err;

  sprintf(path, "/shnet/ledger/%s/%s", type, ledger->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_inode_unlink(fl);
  if (err)
    return (err);

  sprintf(path, "/shnet/ledger/%s/%s.tx", type, ledger->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_inode_unlink(fl);
  if (err)
    return (err);

  return (0);
}
#endif

void free_ledger(tx_ledger_t **ledger_p, tx_t **tx_p)
{

  if (tx_p) {
    free(*tx_p);
    *tx_p = NULL;
  }

  if (ledger_p) {
    free(*ledger_p);
    *ledger_p = NULL;
  } 

}

int process_ledger_tx(tx_app_t *cli, tx_ledger_t *ledger)
{
#if 0
  tx_ledger_t *ent;
  int err;

  ent = (tx_ledger_t *)pstore_load(TX_LEDGER, ledger->ledger_tx.hash);
  if (!ent) {
    err = confirm_ledger(ledger, (tx_t *)ledger->ledger);
    if (err)
      return (err);

    pstore_save(ledger, sizeof(tx_ledger_t));
  }

  return (0);
#endif
}






static ledger_t *ledger_init(void)
{
  ledger_t *l;

  l = (ledger_t *)calloc(1, sizeof(ledger_t));
  if (!l)
    return (NULL);

  l->ledger_buff = shbuf_init();

  return (l);
}

/**
 * @note each ledger is independent of root transaction key and current hour. 
 */
ledger_t *ledger_load(shkey_t *lkey, shtime_t now)
{
  ledger_t *l;
  SHFL *fl;
  char path[SHFS_PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  l = ledger_init();
  if (!l)
    return (NULL);

  sprintf(path, "/sys/net/ledger/%s/%u", 
      shkey_print(lkey), (unsigned int)(shtimef(now)/3600));
  fl = shfs_file_find(sharedaemon_fs(), path);
  shfs_read(fl, l->ledger_buff);

  data_len = shbuf_size(l->ledger_buff);
  if (data_len < sizeof(tx_ledger_t)) {
    tx_ledger_t l_base;

    /* initialize */
    memset(&l_base, 0, sizeof(tx_ledger_t));
    local_transid_generate(TX_LEDGER, &l_base.ledger_tx);
    memcpy(&l_base.ledger_txkey, lkey, sizeof(shkey_t));
    shbuf_clear(l->ledger_buff);
    shbuf_cat(l->ledger_buff, &l_base, sizeof(tx_ledger_t));
  }

  /* populate ledger */
  l->net = (tx_ledger_t *)shbuf_data(l->ledger_buff);
  l->ledger = (tx_t *)(shbuf_data(l->ledger_buff) + sizeof(tx_ledger_t));
  l->net->ledger_height = (shbuf_size(l->ledger_buff) - sizeof(tx_ledger_t)) / sizeof(tx_t);

  return (l);
}

int ledger_save(ledger_t *l)
{
  shfs_t *fs = sharedaemon_fs();
  SHFL *fl;
  char path[SHFS_PATH_MAX+1];
int err;

  sprintf(path, "/sys/net/ledger/%s/%u", 
      shkey_print(&l->net->ledger_txkey),
      (unsigned int)(shtimef(l->net->ledger_tx.tx_stamp)/3600));
  fl = shfs_file_find(fs, path);
  err = shfs_write(fl, l->ledger_buff);
  if (err)
    sherr(err, "save ledger");

#if 0
  if (l->net->ledger_stamp)
    sched_tx(shbuf_data(l->ledger_buff), shbuf_size(l->ledger_buff));
#endif

  return (0);
}

int ledger_close(ledger_t *l)
{
  int err;

  err = 0;
  if (l->flags & LEDGERF_UPDATE)
    err = ledger_save(l);

  shbuf_free(&l->ledger_buff);
  free(l);

  return (err);
}

int ledger_archive(ledger_t *l)
{
  shkey_t *sig_key;
  uint64_t crc;
  uint64_t fee;
  int i;

  /* assign closure time-stamp. */
  l->net->ledger_stamp = shtime();

#if 0
  /* calculate fees */
  fee = 0;
  for (i = 0; i < l->net->ledger_height; i++) {
    fee += l->ledger[i].net.tx_fee;
  }
  l->net->ledger_fee = fee;
#endif

  /* generate signature */
  crc = shcrc(l->net, sizeof(tx_ledger_t));
  sig_key = shkey_cert(&l->net->ledger_txkey, crc, l->net->ledger_stamp);
  memcpy(&l->net->ledger_sig, sig_key, sizeof(shkey_t));
  shkey_free(&sig_key);

  l->flags |= LEDGERF_UPDATE; 

  return (0);
}


int ledger_tx_add(ledger_t *l, tx_t *tx)
{
  tx_ledger_t *cur_l;
  char hash[MAX_SHARE_HASH_LENGTH];
  int err;

  if (tx->tx_op == TX_INIT)
    return (0); /* not recorded */

  /* ledger is stored in 'pstore' based on public peer key */
  strcpy(hash, shkey_print(&l->net->ledger_txkey));
  cur_l = (tx_ledger_t *)pstore_load(TX_LEDGER, hash);
  if (!cur_l ||
      0 != strcasecmp(cur_l->ledger_tx.hash, l->net->ledger_tx.hash)) {
    ledger_archive(l);
    if (cur_l) {
      strcpy(l->net->parent_hash, cur_l->ledger_tx.hash);
      l->net->ledger_seq = cur_l->ledger_seq + 1;
    }
    pstore_save(shbuf_data(l->ledger_buff), shbuf_size(l->ledger_buff));
  }

  l->net->ledger_height++;
  shbuf_cat(l->ledger_buff, tx, sizeof(tx_t));
  l->flags |= LEDGERF_UPDATE; 

fprintf(stderr, "DEBUG: ledger_tx_add: added transaction '%s' (op %d) to ledger '%s' (x%d)\n", tx->hash, tx->tx_op, l->net->ledger_tx.hash, l->net->ledger_height);

  return (0);
}

tx_t *ledger_tx_load(shkey_t *lkey, char *tx_hash, shtime_t tx_stamp)
{
  static tx_t tx;
  ledger_t *l;
  int err;
  int i;

  l = ledger_load(lkey, tx_stamp);
  if (!l)
    return (NULL);

  memset(&tx, 0, sizeof(tx));
  for (i = 0; i < l->net->ledger_height; i++) {
    if (0 == strcasecmp(tx_hash, l->ledger[i].hash)) {
      memcpy(&tx, l->ledger + i, sizeof(tx_t));
      ledger_close(l);
      return (&tx);
    }
  }

  ledger_close(l);
  return (NULL);
}


int ledger_height(ledger_t *l)
{
  if (!l)
    return (0);
  return (l->net->ledger_height);
}
