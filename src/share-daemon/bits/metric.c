
/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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


int inittx_metric(tx_metric_t *met, int type, void *data, size_t *data_len)
{
  shcard_t *card_data;
  shkey_t *key;
  shkey_t *peer_key;
  shkey_t sig_key;
  shtime_t sig_expire;
  char sig_hash[MAX_SHARE_HASH_LENGTH];
  uint64_t sig_csum;
  int err;

  peer_key = NULL;
  sig_csum = 0;
  switch (type) {
    case SHMETRIC_CARD:
      card_data = (shcard_t *)data;
      peer_key = shpeer_kpub(&card_data->card_issuer);
      sig_csum = card_data->card_id;
      sig_expire = card_data->card_expire;
      break;
  }

  /* generate signature */
  key = shkey_cert(peer_key, sig_csum, sig_expire);
  memcpy(&sig_key, key, sizeof(shkey_t));
  shkey_free(&key);

  met->met_type = type;
  met->met_expire = sig_expire;
  memcpy(&met->met_sig, &sig_key, sizeof(shkey_t));

  switch (type) {
    case SHMETRIC_CARD:
      /* fill base data */
      memcpy(&met->met_acc, &card_data->card_acc, sizeof(uint64_t));
      met->met_flags = card_data->card_flags;
      strncpy(met->met_name, card_data->card_type, sizeof(card_data->card_type));
      break;
  }

  err = tx_init(NULL, (tx_t *)met, TX_METRIC); 
  if (err) {
    pstore_free(met);
    return (err);
  }

  return (0);
}

tx_metric_t *alloc_metric(int type, void *data, size_t *data_len)
{
  tx_metric_t *met;

  met = (tx_metric_t *)calloc(1, sizeof(tx_metric_t));
  if (!met)
    return (NULL);

  return (met);
}





int txop_metric_init(shpeer_t *cli_peer, tx_metric_t *met)
{
  return (0);
}

int txop_metric_confirm(shpeer_t *cli_peer, tx_metric_t *met)
{

  if (shtime_after(shtime(), met->met_expire))
    return (SHERR_KEYEXPIRED);

  return (0);
}

int txop_metric_send(shpeer_t *cli_peer, tx_metric_t *met)
{
  return (0);
}

int txop_metric_recv(shpeer_t *cli_peer, tx_metric_t *met)
{
  return (0);
}

