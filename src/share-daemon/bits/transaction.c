
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

#define DEFAULT_SCRYPT_DIFFICULTY 0.001
#define DEFAULT_SCRYPT_NBIT "1f3e8fff"

static void _fcrypt_generate_transaction_id(tx_t *tx)
{
  shkey_t *key;
	int step = 10240;
  unsigned int idx;
  uint64_t best_crc;
  uint64_t crc;
  int crc_once;
int nonce;

  memset(tx->hash, 0, sizeof(tx->hash));

  if (step < MIN_TX_ONCE)
    step = MIN_TX_ONCE;
  else if (step > (MAX_TX_ONCE / 2))
    step = MAX_TX_ONCE / 2;

  best_crc = 0;
  crc_once = -1;
  nonce = 0;
//  printf("starting..\n");
//  tx->tx_stamp = shtime();
  for (idx = step; idx < MAX_TX_ONCE; idx += step) {
    tx->nonce = idx; 
    crc = shcrc(tx, sizeof(tx_t)); 
    if (!best_crc || crc < best_crc) {
      best_crc = crc;
      crc_once = idx;
      nonce = 0;
    } else {
      nonce++;
      if (nonce > step)
        break;
    }
  }
//printf("ending @ %d (once %d)..\n", idx, crc_once);

  key = shkey_bin((char *)&best_crc, sizeof(best_crc)); 
  sprintf(tx->hash, "%-64.64s", shkey_hex(key));
  shkey_free(&key);

  tx->nonce = crc_once;
#if 0
	tx->tx_method = TXHASH_FCRYPT;
#endif
  //tx->tx_id = best_crc;

}

static int _scrypt_generate_transaction_id(tx_t *tx, char *phash, char *hash, char **merkle_list)
{
  const uint32_t *ostate;
	scrypt_peer speer;
	scrypt_work work;
	char nonce1[256];
	char nbit[256];
  char cb1[256];
  time_t now;
  char ntime[64];
  char target[32];
  int err;
  int i;

  //tx->tx_stamp = shtime();

	memset(&work, 0, sizeof(work));

	memset(&speer, 0, sizeof(speer));
	sprintf(nonce1, "%-8.8x", 0);
	shscrypt_peer(&speer, nonce1, DEFAULT_SCRYPT_DIFFICULTY);
  sprintf(work.xnonce2, "%-8.8x", 0x0);
  strcpy(nbit, DEFAULT_SCRYPT_NBIT);
//  strcpy(cb1, shkey_hex(get_tx_key_root(tx)));
  strcpy(cb1, shkey_hex(get_tx_key(tx)));
  now = shutime(tx->tx_stamp);
  sprintf(ntime, "%-8.8x", (unsigned int)now); 
  shscrypt_work(&speer, &work, merkle_list, phash, cb1, hash, nbit, ntime);
  err = shscrypt(&work, 10240);
  if (err) {
    PRINT_ERROR(err, "_scrypt_generate_transaction_id");
    return (err);
	}

	ostate = (uint32_t *)work.hash;
  memset(tx->hash, 0, sizeof(tx->hash));
  for (i = 0; i < 8; i++)
    sprintf(tx->hash+strlen(tx->hash), "%-8.8x", ostate[i]);
	tx->nonce = work.hash_nonce;
	tx->tx_alg = TXHASH_SCRYPT;

	return (0);
}

#if 0
int generate_transaction_id(int tx_op, tx_t *tx, char *hash)
{
  shpeer_t *peer;
  char *merkle_list[4];
  char peer_hash[256];
	int err;
  int i;

  peer = sharedaemon_peer();
  sprintf(peer_hash, "%s", shkey_hex(shpeer_kpub(peer)));

  for (i = 0; i < 4; i++)
    merkle_list[i] = NULL;

  merkle_list[0] = peer_hash;
  if (hash) {
    merkle_list[1] = hash;
  }

  /* The time-stamp of when the transaction originated. */
  tx->tx_stamp = (uint64_t)shtime();

  /* operation mode being referenced. */
  tx->tx_op = tx_op;

  /* record originating peer */
  memcpy(&tx->tx_peer, shpeer_kpub(peer), sizeof(shkey_t)); 

	err = _scrypt_generate_transaction_id(tx, NULL, NULL, merkle_list);
	if (err)
		return (err);

	return (0);
}
#endif

int local_transid_generate(int tx_op, tx_t *tx)
{
  shpeer_t *peer;
  char **merkle_list;
  shtime_t now;
  ledger_t *l;
  int err;
  int i;

  now = shtime();

  if (tx->tx_stamp == SHTIME_UNDEFINED) {
    /* The time-stamp of when the transaction originated. */
    tx->tx_stamp = (uint64_t)now;
  }

  /* operation mode being referenced. */
  tx->tx_op = tx_op;

  /* originating peer */
  peer = sharedaemon_peer();
  memcpy(&tx->tx_peer, shpeer_kpriv(peer), sizeof(shkey_t)); 

  if (tx_op == TX_LEDGER) {
    err = _scrypt_generate_transaction_id(tx, NULL, NULL, NULL);
    if (err)
      return (err);
  } else {
/* todo: load last archived ledger */ 
    l = ledger_load(peer, shtime_adj(now, -3600));
    if (l) {
      merkle_list = (char **)calloc(l->net->ledger_height + 1, sizeof(char **));
      for (i = 0; i < l->net->ledger_height; i++) {
        merkle_list[i] = l->ledger[i].hash;
      }
      err = _scrypt_generate_transaction_id(tx, 
          l->net->parent_hash, l->net->ledger_tx.hash, merkle_list);
      free(merkle_list);
      ledger_close(l);
      if (err)
        return (err);
    }
  }

	return (0);
}




#if 0
int has_tx_access(tx_id_t *id, tx_t *tx)
{
  shpeer_t *lcl_peer;
  tx_id_t lcl_id;

	if (tx->tx_group == TX_GROUP_PRIVATE) {
		lcl_peer = shpeer();
		if (!shkey_cmp(shpeer_kpub(lcl_peer), &tx->tx_peer)) {
			/* transaction did not originate from local node. */
			return (FALSE);
		}
	} else if (tx->tx_group == TX_GROUP_PEER) {
    memset(&lcl_id, 0, sizeof(lcl_id));
#if 0
		get_identity_id(&lcl_id);
#endif
		if (!shkey_cmp(&id->id_sig.sig_peer, &lcl_id.id_sig.sig_peer)) {
			/* transaction did not originate from peer group. */
			return (FALSE);
		}
  }

  return (TRUE);
}
#endif



/**
 * Fill in the neccessary network components of a transaction header.
 */
int prep_net_tx(tx_t *tx, tx_net_t *net, shkey_t *sink, size_t size)
{

  memset(net, '\000', sizeof(tx_net_t));

  /* Time-stamp of when transaction was prepared for transmission. */
  net->tx_stamp = shtime();

  /* checksum of transaction header */
  net->tx_crc = (uint32_t)shcrc(tx, sizeof(tx_t));

  if (!sink)
    sink = ashkey_blank();
  memcpy(&net->tx_sink, sink, sizeof(net->tx_sink));

  net->tx_magic = SHMEM_MAGIC;
 
  net->tx_size = htonl(size);
}


