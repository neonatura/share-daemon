
/*
 * @copyright
 *
 *  Copyright 20155555 Brian Burrell 
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

int get_bond_state(tx_bond_t *bond)
{
  return (bond->bond_state);
}

void set_bond_state(tx_bond_t *bond, int state)
{
  if (!bond)
    return;
  if (bond->bond_state == state)
    return;
  bond->bond_state = state;

  /* remotely inform connected peers that bond state has updated. */
  local_broadcast_bond(bond);

  /* save the new state info persistently. */
  save_bond(bond);
}


shkey_t *get_bond_key(shkey_t *sender, shkey_t *receiver, shkey_t *ref)
{
  static shkey_t ret_key;
  shkey_t keys[3];
  shkey_t *key;

  if (!sender)
    sender = shpeer_kpriv(sharedaemon_peer());
  else if (!receiver)
    receiver = shpeer_kpriv(sharedaemon_peer());

  if (!ref)
    ref = ashkey_blank();

  memcpy(&keys[0], sender, sizeof(shkey_t));
  memcpy(&keys[1], receiver, sizeof(shkey_t));
  memcpy(&keys[2], ref, sizeof(shkey_t));

  key = shkey_bin(keys, sizeof(shkey_t) * 3);
  memcpy(&ret_key, key, sizeof(ret_key));
  shkey_free(&key);

  return (&ret_key);
}

tx_bond_t *load_bond(shkey_t *bond_key)
{
  return ((tx_bond_t *)pstore_load(TX_FILE, shkey_hex(bond_key)));
}

/**
 * @param origin a peer reference to the sender of the bond
 */
tx_bond_t *load_bond_peer(shpeer_t *sender, shpeer_t *receiver, shpeer_t *ref)
{
  return (load_bond(get_bond_key(shpeer_kpriv(sender), shpeer_kpriv(receiver), shpeer_kpub(ref))));  
}

void save_bond(tx_bond_t *bond)
{
  pstore_save(bond, sizeof(tx_bond_t));
}

void free_bond(tx_bond_t **bond_p)
{
  tx_bond_t *bond;

  if (!bond_p)
    return;

  bond = *bond_p;
  *bond_p = NULL;

  if (!bond)
    return;

  pstore_free(bond);
}

int local_broadcast_bond(tx_bond_t *bond)
{
  sched_tx(bond, sizeof(tx_bond_t) + bond->ino_size);
  return (0);
}

int remote_broadcast_bond(shpeer_t *origin, tx_bond_t *bond)
{
  sched_tx_sink(shpeer_kpriv(origin),  bond, sizeof(tx_bond_t));
  return (0);
}


int local_confirm_bond(tx_bond_t *bond)
{

  return (0);
}

int remote_validate_bond(tx_app_t *cli, tx_bond_t *bond)
{
  int err;

  /* verify bond signature */
  err = validate_bond_signature(bond);
  if (err) {
//    cli->stinky++;
    return (err);
  }

  return (0);
}

/**
 * A bond notification received from a client on the local machine.
 */
int local_bond_notification(shpeer_t *peer, shfs_hdr_t *blk)
{

  return (0);
}


/**
 * An incoming TXFILE_CHECKSUM file operation from another server.
 */
int remote_bond_notification(shpeer_t *origin, tx_bond_t *tx)
{
return (0);
}

static void generate_bond_signature(tx_bond_t *bond)
{
  shkey_t *sig_key;
  uint64_t crc;

  crc = (uint64_t)shcrc(bond->bond_sink, strlen(bond->bond_sink));
  sig_key = shkey_cert(&bond->bond_key, crc, bond->bond_expire);
  memcpy(&bond->bond_sig, sig_key, sizeof(shkey_t));
  shkey_free(&sig_key);
}

static int validate_bond_signature(tx_bond_t *bond)
{
  uint64_t crc;
  int err;

  crc = (uint64_t)shcrc(bond->bond_sink, strlen(bond->bond_sink));
  err = shkey_verify(&bond->bond_sig, crc, &bond->bond_key, bond->bond_expire);
  if (err)
    return (err);

  return (0);
}

/**
 * Creates a new bond with the local shared as sender.
 * @note Set the bond state to confirm to initiate transaction.
 */
tx_bond_t *create_bond(shkey_t *bond_key, double duration, double fee, double basis)
{

  bond = (tx_bond_t *)calloc(1, sizeof(tx_bond_t));
  if (!bond)
    return (SHERR_NOMEM);

  local_transid_generate(TX_BOND, &bond->tx);

  bond->bond_stamp = shtime64();
  bond->bond_stamp = shtime_adj(bond->bond_stamp, duration);
  bond->bond_credit = (uint64_t)(fee / 0.00000001);
  bond->bond_basis = (uint32_t)(basis * 10000);
  bond->bond_state = TXBOND_PENDING;

  /* authenticate bond info */ 
  generate_bond_signature(bond);

  err = tx_init(NULL, (tx_t *)bond, TX_BOND);
  if (err)
    return (err);

  return (bond);
}

tx_bond_t *create_bond_peer(shpeer_t *receiver, shpeer_t *ref, double duration, double fee, double basis)
{
  return (create_bond(get_bond_key(NULL, receiver, ref),
        duration, fee, basis));
}


int process_bond_tx(tx_app_t *cli, tx_bond_t *bond)
{
  tx_bond_t *ent;
  int err;

  err = remote_validate_bond(cli, bond);
  if (err)
    return (err);

  ent = (tx_bond_t *)pstore_load(TX_BOND, bond->bond_tx.hash);
  if (!ent)
    return (0); /* no state info */

  switch (bond->bond_state) {
/* .. */
  }

  return (0);
}


/**
 * Confirm a portion of the bond value as confirmed required payment.
 * @note Set by the bond's receiver. Does not inform remote peers.
 */
int confirm_bond_value(tx_bond_t *bond, double fee)
{

  if (get_bond_state(bond) != TXBOND_CONFIRM) {
    /* invalid bond state */
    return (SHERR_INVAL);
  }

  bond->value = MIN(bond->value + (fee / 0.0000001), bond->credit);

  return (0);
}

/**
 * Complete a bond.
 * @note No further accumulation of bond value occurs.
 */
int complete_bond(tx_bond_t *bond) 
{

  if (get_bond_state(bond) != TXBOND_CONFIRM) {
    /* invalid bond state */
    return (SHERR_INVAL);
  }

  set_bond_state(bond, TXBOND_FINAL);

  return (0);
}



int txop_bond_init(shpeer_t *cli_peer, tx_bond_t *bond)
{
  int err;

  if (!bond)
    return (SHERR_INVAL);

  err = generate_bond_signature(bond);
  if (err)
    return (err);

  return (0);
}

int txop_bond_confirm(shpeer_t *cli_peer, tx_bond_t *bond)
{
  int err;

  if (!bond)
    return (SHERR_INVAL);

  err = validate_bond_signature(bond);
  if (err)
    return (err);

  return (0);
}

int txop_bond_recv(shpeer_t *cli_peer, tx_bond_t *bond)
{

  switch (bond->bond_state) {
  }

  return (0);
}

int txop_bond_send(shpeer_t *cli_peer, tx_bond_t *bond)
{
  return (0);
}
