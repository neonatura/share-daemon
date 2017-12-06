
/*
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
 */  

#define __BITS_C__

#include "sharedaemon.h"
#include <stddef.h>

#define TXOP(_f) (txop_f)&_f
static txop_t _txop_table[MAX_TX] = {
  { "none" },
  { "init", sizeof(tx_init_t), offsetof(tx_init_t, ini_endian),
    TXOP(txop_init_init), TXOP(txop_init_confirm), 
    TXOP(txop_init_recv), TXOP(txop_init_send) },
  { "subscribe", sizeof(tx_subscribe_t), 0,
    TXOP(txop_sub_init), TXOP(txop_sub_confirm),
    TXOP(txop_sub_recv), NULL, TXOP(txop_sub_wrap) },
  { "ident", sizeof(tx_id_t), 0,
    TXOP(txop_ident_init), TXOP(txop_ident_confirm), 
    TXOP(txop_ident_recv), TXOP(txop_ident_send) },
  { "account", sizeof(tx_account_t), offsetof(tx_account_t, acc_peer),
    TXOP(txop_account_init), TXOP(txop_account_confirm),
    TXOP(txop_account_recv), TXOP(txop_account_send),
    TXOP(txop_account_wrap) },
  { "session", sizeof(tx_session_t), offsetof(tx_session_t, sess_key),
    TXOP(txop_session_init), TXOP(txop_session_confirm), 
    TXOP(txop_session_recv), TXOP(txop_session_send) },
  { "app", sizeof(tx_app_t), offsetof(struct tx_app_t, app_stamp),
    TXOP(txop_app_init), TXOP(txop_app_confirm),
    TXOP(txop_app_recv), TXOP(txop_app_send) },
  { "file", sizeof(struct tx_file_t), offsetof(struct tx_file_t, ino_mtime), 
    TXOP(txop_file_init), TXOP(txop_file_confirm), 
    TXOP(txop_file_recv), TXOP(txop_file_send) },
  { "ward", sizeof(tx_ward_t), 0,
    TXOP(txop_ward_init), TXOP(txop_ward_confirm), 
    TXOP(txop_ward_recv), TXOP(txop_ward_send) },
  { "trust", sizeof(tx_trust_t), offsetof(struct tx_trust_t, trust_sig),
    TXOP(txop_trust_init), TXOP(txop_trust_confirm) },
  { "ledger", sizeof(tx_ledger_t) },
  { "license", sizeof(tx_license_t), 0,
    TXOP(txop_lic_init), TXOP(txop_lic_confirm), 
    TXOP(txop_lic_recv), TXOP(txop_lic_send) },
  { "metric", sizeof(tx_metric_t), 0,
    TXOP(txop_metric_init), TXOP(txop_metric_confirm), 
    TXOP(txop_metric_recv), TXOP(txop_metric_send) },
  { "mem", sizeof(tx_mem_t), 0 },
  { "vm", sizeof(tx_vm_t), 0,
    TXOP(txop_vm_init), TXOP(txop_vm_confirm),
    TXOP(txop_vm_recv), TXOP(txop_vm_send) },
  { "run", sizeof(tx_run_t), 0,
    TXOP(txop_run_init), TXOP(txop_run_confirm),
    TXOP(txop_run_recv), TXOP(txop_run_send) },
  { "wallet", sizeof(tx_wallet_t), offsetof(struct tx_wallet_t, wal_stamp),
    TXOP(txop_wallet_init), TXOP(txop_wallet_confirm),
    TXOP(txop_wallet_recv), TXOP(txop_wallet_send), TXOP(txop_wallet_wrap) },
  { "bond", sizeof(tx_bond_t) },
  { "asset", sizeof(tx_asset_t), 0,
    &txop_asset_init, &txop_asset_confirm },
  { "contract", sizeof(tx_contract_t), 0, },
  { "event", sizeof(tx_event_t), 0,
    TXOP(txop_event_init), TXOP(txop_event_confirm), 
    TXOP(txop_event_recv), TXOP(txop_event_send) },
  { "eval", sizeof(tx_eval_t), offsetof(struct tx_eval_t, eval_sig),
    TXOP(txop_eval_init), TXOP(txop_eval_confirm),
    TXOP(txop_eval_recv), TXOP(txop_eval_send) },
  { "context", sizeof(tx_context_t), offsetof(struct tx_context_t, ctx_sig),
    TXOP(txop_context_init), TXOP(txop_context_confirm), 
    TXOP(txop_context_recv), TXOP(txop_context_send), TXOP(txop_context_wrap) },
  { "reference", sizeof(tx_ref_t), 0,
    TXOP(txop_ref_init), TXOP(txop_ref_confirm),
    TXOP(txop_ref_recv), TXOP(txop_ref_send), TXOP(txop_ref_wrap) },
  { "clock", sizeof(tx_clock_t), offsetof(struct tx_clock_t, clo_send),
    TXOP(txop_clock_init), TXOP(txop_clock_confirm),
    TXOP(txop_clock_recv), TXOP(txop_clock_send) },
};

txop_t *get_tx_op(int tx_op)
{
  if (tx_op < 0 || tx_op >= MAX_TX)
    return (NULL);

  return (&_txop_table[tx_op]);
}

int confirm_tx_key(txop_t *op, tx_t *tx)
{
  shkey_t *c_key = &tx->tx_key;
  shkey_t *key;
  shbuf_t *buff;
  tx_t *k_tx;
  size_t len;
  int confirm;

  if (!op || !tx)
    return (SHERR_INVAL);

  if (op->op_size == 0)
    return (0);

  /* allocate working copy */
  len = MAX(sizeof(tx_t), op->op_keylen ? op->op_keylen : op->op_size);
  buff = shbuf_init();
  shbuf_cat(buff, (char *)tx, len);

  /* blank out tx key */
  k_tx = (tx_t *)shbuf_data(buff);
  memset(&k_tx->tx_key, '\000', sizeof(k_tx->tx_key));

  /* verify generated tx key matches. */
  key = shkey_bin(shbuf_data(buff), shbuf_size(buff));
  confirm = shkey_cmp(c_key, key);
  shkey_free(&key);
  shbuf_free(&buff);

  if (!confirm) {
    return (SHERR_INVAL);
  }

  return (0);
}


int tx_confirm(shpeer_t *cli_peer, tx_t *tx)
{
  txop_t *op;  
  uint32_t tx_op;
  int err;


  op = get_tx_op(tx->tx_op);
  if (!op)
    return (SHERR_INVAL);

  if (op->op_size == 0)
    return (0);


  /* verify tx key reflect transaction received. */
  err = confirm_tx_key(op, tx);
if (err) fprintf(stderr, "DEBUG: tx_confirm: %d = confirm_tx_key() [op %d]\n", err, tx->tx_op);
  if (err)
    return (err);

  if (op->op_conf) {
    /* transaction-level integrity verification */
    err = op->op_conf(cli_peer, tx);
    if (err) {
fprintf(stderr, "DEBUG: tx_confirm: %d = op_confirm()\n", err);
      return (err);
}
  }

  return (0);
}

static int is_tx_stored(int tx_op)
{
  int ret_val;

  ret_val = TRUE;
  switch (tx_op) {
    case TX_SUBSCRIBE:
    case TX_IDENT:
    case TX_SESSION:
    case TX_ACCOUNT:
    case TX_APP:
    case TX_REFERENCE:
    case TX_CLOCK:
      ret_val = FALSE;
      break;
  }

return (ret_val);
}

int tx_recv(shpeer_t *cli_peer, tx_t *tx)
{
  tx_ledger_t *l;
  tx_t *rec_tx;
  txop_t *op;
  int err;

  if (!tx)
    return (SHERR_INVAL);

#if 0
  if (ledger_tx_load(shpeer_kpriv(cli_peer), tx->hash, tx->tx_stamp)) {
fprintf(stderr, "DEBUG: tx_recv: skipping duplicate tx '%s'\n", tx->hash); 
    return (SHERR_NOTUNIQ);
}
#endif

  op = get_tx_op(tx->tx_op);
  if (!op)
    return (SHERR_INVAL);

  if (tx->tx_flag & TXF_WARD) {
    err = txward_confirm(tx); 
    if (err)
      return (err);
  }


/* check for dup in ledger (?) */

  rec_tx = (tx_t *)tx_load(tx->tx_op, get_tx_key(tx));
  if (!rec_tx) {
    rec_tx = (tx_t *)calloc(1, op->op_size);
    if (!rec_tx)
      return (SHERR_NOMEM);

    memcpy(rec_tx, tx, op->op_size);
#if 0 
    err = tx_init(cli_peer, rec_tx);
    if (err) {
      pstore_free(rec_tx);
      return (err);
    }
#endif

    err = tx_save(rec_tx);
    if (err) {
      pstore_free(rec_tx);
      return (err);
    }
  }

  if (op->op_recv) {
    err = op->op_recv(cli_peer, tx);
    if (err) {
      pstore_free(rec_tx);
      return (err);
    }
  }

  pstore_free(rec_tx);

  if (is_tx_stored(tx->tx_op)) {
    l = ledger_load(shpeer_kpriv(cli_peer), shtime());
    if (l) {
      ledger_tx_add(l, tx);
      ledger_close(l);
    }
  }

  return (0);
}

int tx_init(shpeer_t *cli_peer, tx_t *tx, int tx_op)
{
  shkey_t *key;
  txop_t *op;
  int tx_len;
  int err;

  if (!tx)
    return (SHERR_INVAL);

  op = get_tx_op(tx_op);
  if (!op)
    return (SHERR_INVAL);

  if (op->op_size == 0)
    return (0);

  tx->tx_op = tx_op;

  if (op->op_init) {
    err = op->op_init(cli_peer, tx);
    if (err)
      return (err);
  }

  if (tx->tx_flag & TXF_WARD) {
    err = txward_init(tx); 
    if (err)
      return (err);
  }

  err = local_transid_generate(tx_op, tx);
  if (err)
    return (err); /* scrypt error */

  /* generate transaction key */
  memcpy(&tx->tx_key, ashkey_blank(), sizeof(tx->tx_key));
  key = shkey_bin((char *)tx, op->op_keylen ? op->op_keylen : op->op_size);
  memcpy(&tx->tx_key, key, sizeof(tx->tx_key));
  shkey_free(&key);
 
  return (0);
}

int tx_send(shpeer_t *cli_peer, tx_t *tx)
{
  unsigned char *data = (unsigned char *)tx;
  size_t data_len;
  tx_ledger_t *l;
  int err;
  txop_t *op;

  if (!data)
    return (SHERR_INVAL);

  data_len = get_tx_size(tx);
  if (!data_len)
    return (0);

  op = get_tx_op(tx->tx_op);
  if (!op)
    return (SHERR_INVAL);

  if (op->op_send) {
    err = op->op_send(cli_peer, data);
    if (err) {
      if (err == SHERR_OPNOTSUPP)
        return (0);

      return (err);
    }
  }

  if (is_tx_stored(tx->tx_op)) {
    l = ledger_load(shpeer_kpriv(sharedaemon_peer()), shtime());
    if (l) {
      ledger_tx_add(l, (tx_t *)data);   
      ledger_close(l);
    }
  }

#if 0
  /* encapsulate for network transfer. */
  tx_wrap(shpeer_kpriv(cli_peer), (tx_t *)tx);
#endif

  if (cli_peer) {
    sched_tx_sink(shpeer_kpriv(cli_peer), data, data_len);
  } else {
    sched_tx(data, data_len);
  }

  return (0);
}



shkey_t *get_tx_key(tx_t *tx)
{
  tx_license_t *lic;
  tx_file_t *ino;
  tx_app_t *app;
  tx_ward_t *ward;
  tx_bond_t *bond;
  tx_account_t *acc;
  tx_session_t *sess;
  tx_context_t *ctx;
  tx_id_t *id;
  shkey_t *ret_key;

  if (!tx)
    return (ashkey_blank());

  ret_key = NULL;
  switch (tx->tx_op) {
    case TX_LICENSE:
      lic = (tx_license_t *)tx;
      //ret_key = &lic->lic.lic_sig;
      ret_key = &lic->lic.esig.id;
      break;
    case TX_FILE:
      ino = (tx_file_t *)tx;
      ret_key = &ino->ino_name;
      break;
#if 0
    case TX_APP:
      app = (tx_app_t *)tx;
      ret_key = &app->app_sig;
      break;
#endif
    case TX_BOND:
      bond = (tx_bond_t *)tx;
      ret_key = &bond->bond_sig;
      break;
#if 0
    case TX_WARD:
      ward = (tx_ward_t *)tx;
      ret_key = &ward->ward_sig.sig_key;
      break;
#endif
    case TX_IDENT:
      id = (tx_id_t *)tx;
      ret_key = &id->id_key;
      break;
#if 0
    case TX_ACCOUNT:
      acc = (tx_account_t *)tx;
      ret_key = &acc->pam_seed.seed_sig;
      break;
#endif
#if 0
    case TX_SESSION:
      sess = (tx_session_t *)tx;
      ret_key = &sess->sess_key;
      break;
#endif
    case TX_CONTEXT:
      ctx = (tx_context_t *)tx;
      ret_key = &ctx->ctx_name;
      break;
    default:
      ret_key = &tx->tx_key;
      break;
  }

  return (ret_key);
}

#if 0
/**
 * @returns An allocated parent transaction or NULL.
 */
tx_t *get_tx_parent(tx_t *tx)
{
  tx_t *p_tx;

  if (0 == shkey_cmp(&tx->tx_pkey, ashkey_blank()))
    return (NULL);

  return ((tx_t *)pstore_load(tx->tx_ptype, shkey_hex(&tx->tx_pkey)));
}
#endif

#if 0
/**
 * Obtain the root transaction key of the transaction hierarchy.
 * @returns An unallocated tx key or NULL if tx is the root tx.
 */
shkey_t *get_tx_key_root(tx_t *tx)
{
  static shkey_t ret_key;
  tx_t *t_tx;
  tx_t *p_tx;

  t_tx = tx;
  memcpy(&ret_key, ashkey_blank(), sizeof(ret_key));
  while ((p_tx = get_tx_parent(t_tx))) {
    if (t_tx != tx)
      pstore_free(t_tx);

    t_tx = p_tx;
    memcpy(&ret_key, get_tx_key(t_tx), sizeof(ret_key));
  }

  return (&ret_key);
}
#endif

const char *get_tx_label(int tx_op)
{
  static const char *unknown_str = "unknown";
  txop_t *op;

  op = get_tx_op(tx_op);
  if (!op)
    return (unknown_str);

  return (op->op_name);
}

size_t get_tx_size(tx_t *tx)
{
  tx_asset_t *ass;
  tx_file_t *ino;
  txop_t *op;
  size_t ret_size;
  
  if (!tx)
    return (0);

  ret_size = 0;
  op = get_tx_op(tx->tx_op);
  if (!op)
    return (0);

  switch (tx->tx_op) {
    case TX_FILE:
      ino = (tx_file_t *)tx;
      if (ino->ino_op != TXFILE_READ)
        ret_size = op->op_size + ino->seg_len;
      else
        ret_size = op->op_size;
      break;
    case TX_ASSET:
      ass = (tx_asset_t *)tx;
      ret_size = op->op_size + ass->ass_size;
      break;
    default:
      ret_size = op->op_size;
      break;
  }

  return (ret_size);
}


/**
 * @param data_len A byte size between 2 and 4096 bytes.
 */
void wrap_bytes(void *data, size_t data_len)
{
  uint32_t n_data[1024];
  uint32_t *i_data;
  size_t sw_len;
  int sw_data_idx;
  int tot;
  int i;

  if (htonl(SHMEM_ENDIAN_MAGIC) == SHMEM_ENDIAN_MAGIC)
    return; /* data is in network-byte order */

  if (data_len < sizeof(uint16_t))
    return; /* all done */

  if (data_len < sizeof(uint32_t)) {
    uint16_t *t_data = (uint16_t *)data;
    *t_data = htons(*t_data);
    return;
  }

  /* swap */
  tot = MIN(1024, data_len / 4);
  i_data = (uint32_t *)data;
  for (i = 0; i < tot; i++) {
    n_data[tot-i-1] = htonl(i_data[i]);
  }

  memcpy(i_data, n_data, tot * sizeof(uint32_t));
}

void unwrap_bytes(void *data, size_t data_len)
{
  uint32_t n_data[1024];
  uint32_t *i_data;
  size_t sw_len;
  int sw_data_idx;
  int tot;
  int i;

  if (htonl(SHMEM_ENDIAN_MAGIC) == SHMEM_ENDIAN_MAGIC)
    return; /* data is in network-byte order */

  if (data_len < sizeof(uint16_t))
    return; /* all done */

  if (data_len < sizeof(uint32_t)) {
    uint16_t *t_data = (uint16_t *)data;
    *t_data = htons(*t_data);
    return;
  }

  /* swap */
  tot = MIN(1024, data_len / 4);
  i_data = (uint32_t *)data;
  for (i = 0; i < tot; i++) {
    n_data[tot-i-1] = ntohl(i_data[i]);
  }

  memcpy(i_data, n_data, tot * sizeof(uint32_t));
}


void *tx_load(int tx_op, shkey_t *tx_key)
{

  if (!is_tx_stored(tx_op))
    return (NULL);

  return (pstore_load(tx_op, shkey_hex(tx_key)));
}

int tx_save(void *raw_tx)
{
  tx_t *tx = (tx_t *)raw_tx;
  int err;

  if (!is_tx_stored(tx->tx_op))
    return (0);

  err = pstore_save(raw_tx, get_tx_size(tx));
  if (err)
    return (err);

  return (0);
}


void tx_wrap(shpeer_t *cli_peer, tx_t *tx)
{
  txop_t *op;
  int err;
  
  if (!tx)
    return;

  op = get_tx_op(tx->tx_op);
  if (!op)
    return;
 
  if (!op->op_wrap)
    return;

  op->op_wrap(cli_peer, tx);
}


