
/*
 * @copyright
 *
 *  Copyright 2016 Brian Burrell 
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
#include <stddef.h>

#define INIT_TX_KEY(_ini) \
  shkey_bin((char *)(_ini) + offsetof(struct tx_init_t, ini_peer), \
      sizeof(struct tx_init_t) - offsetof(struct tx_init_t, ini_peer))

int prep_init_tx(tx_init_t *ini)
{
  shkey_t *key;
  int err;

  memcpy(&ini->ini_peer, sharedaemon_peer(), sizeof(ini->ini_peer));
  ini->ini_ver = SHARENET_PROTOCOL_VERSION;
  ini->ini_endian = SHMEM_ENDIAN_MAGIC;

#if 0
  key = INIT_TX_KEY(ini);
  if (!key)
    return (SHERR_NOMEM);

  /* fill init verification hash */
  memset(ini->ini_hash, '\000', sizeof(ini->ini_hash));
  strncpy(ini->ini_hash, shkey_print(key), sizeof(ini->ini_hash)-1);
  shkey_free(&key);
#endif

fprintf(stderr,
 "DEBUG: prep_init_tx: peer(%s) endian(%d) ver(%d) seq(%d) stamp(%llu)\n", 
shpeer_print(&ini->ini_peer),
 ini->ini_endian, ini->ini_seq, 
(unsigned long long)ini->ini_stamp);
  

  return (0);
}

#if 0
int confirm_init_tx(tx_init_t *ini)
{
  shkey_t *key;
  char hash[MAX_SHARE_HASH_LENGTH];
  int ok;

fprintf(stderr, "DEBUG: confirm_init_tx: peer '%s'\n", shpeer_print(&ini->ini_peer));
fprintf(stderr, "DEBUG: confirm_init_tx: endian %d\n", (int)ini->ini_endian);
fprintf(stderr, "DEBUG: confirm_init_tx: ver %d\n", (int)ini->ini_ver);
fprintf(stderr, "DEBUG: confirm_init_tx: seq %d\n", (int)ini->ini_seq);
fprintf(stderr, "DEBUG: confirm_init_tx: __reserved__ %d\n", (int)ini->__reserved_1__);
fprintf(stderr, "DEBUG: confirm_init_tx: init_stamp %llu\n", (unsigned long long)ini->ini_stamp);


  key = INIT_TX_KEY(ini);
  if (!key)
    return (SHERR_NOMEM);

  memset(hash, 0, sizeof(hash));
  strncpy(hash, shkey_print(key), sizeof(hash) - 1); 
  ok = (0 == strcmp(hash, ini->ini_hash));
  if (!ok) {
fprintf(stderr, "DEBUG: confirm_init_tx: !HASH: gen'd key: '%s'\n", shkey_print(key));
fprintf(stderr, "DEBUG: confirm_init_tx: !HASH: tx key: '%s'\n", ini->ini_hash);
}
  shkey_free(&key);
  if (!ok) {
    return (SHERR_ACCESS);
}

  return (0);
}
#endif



/**
 * subscribe to all recent tx keys from recent server.
 */
void process_init_ledger_notify(shd_t *cli, tx_init_t *ini)
{
  tx_subscribe_t sub;
  ledger_t *l;
  shd_t *n_cli;
  tx_t *tx;
  shpeer_t *peer;
  shkey_t *key;
  void *tx_data;
  shtime_t now;
  shtime_t t;
  int tot;
  int idx;

fprintf(stderr, "DEBUG: process_init_ledger_notify()\n");

  for (n_cli = sharedaemon_client_list; n_cli; n_cli->next) {
    memset(&sub, 0, sizeof(sub));
    inittx_subscribe(&sub, shpeer_kpriv(&cli->peer), TX_APP, SHOP_LISTEN);
    sched_tx_sink(shpeer_kpriv(&cli->peer), &sub, sizeof(sub));
  }

  now = shtime();
  if (cli->app.app_stamp)
    t = cli->app.app_stamp;
  if (t == SHTIME_UNDEFINED)
    t = now;
//t = shtime_adj(now, -3600); /* DEBUG: */

  peer = sharedaemon_peer();
  while (t < now) {
    l = ledger_load(peer, t);
    t = shtime_adj(t, ONE_HOUR);

    if (l) {
      tot = ledger_height(l);
      for (idx = 0; idx < tot; idx++) {
        tx = l->ledger + idx;
        if (tx->tx_op == TX_LEDGER)
          continue;

        tx_data = pstore_load(tx->tx_op, tx->hash);
        if (!tx_data)
          continue; /* nerp */

        key = get_tx_key(tx);
        if (key)
          continue;

        memset(&sub, 0, sizeof(sub));
        inittx_subscribe(&sub, key, tx->tx_op, SHOP_LISTEN);
        sched_tx_sink(shpeer_kpriv(&cli->peer), &sub, sizeof(sub));
      }
      ledger_close(l);
    }
  }

}

/**
 * Informs a client about all actively known registered apps.
 */
void process_init_app_notify(shd_t *cli, tx_init_t *ini)
{
  shkey_t c_key;
  shd_t *n_cli;

fprintf(stderr, "DEBUG: process_init_app_notify()\n");

  memcpy(&c_key, shpeer_kpriv(&cli->peer), sizeof(c_key));

  for (n_cli = sharedaemon_client_list; n_cli; n_cli = n_cli->next) {
    if (n_cli->app.app_stamp == SHTIME_UNDEFINED)
      continue; /* has no app info */
    if (shkey_cmp(&c_key, shpeer_kpriv(&n_cli->peer)))
      continue; /* don't notify app of themselves */
    if (cli->flags & SHD_CLIENT_AUTH)
      continue; /* not registered */

    sched_tx_sink(shpeer_kpriv(&cli->peer), &n_cli->app, sizeof(tx_app_t));
  }

}

#if 0
int process_init_tx(shd_t *cli, tx_init_t *ini)
{
  tx_app_t *ent;
  shtime_t stamp;
  int err;

  err = confirm_init_tx(ini);
fprintf(stderr, "DEBUG: process_init_tx: %d = confirm_init_tx()\n", err);
  if (err)
    return (err);

  ini->ini_seq++;

  err = prep_init_tx(ini);
fprintf(stderr, "DEBUG: process_init_tx: %d = prep_init_tx()\n", err);
  if (err)
    return (err);

fprintf(stderr, "DEBUG: ini_seq %d\n", ini->ini_seq);
  stamp = ini->ini_stamp;
  switch (ini->ini_seq) {
    case 1:
    case 2:
      /* public peer notification */
      if (sharedaemon_client_find(shpeer_kpriv(&ini->ini_peer)))
        return (SHERR_NOTUNIQ);

      /* establish broadcast channels */
      cli->flags |= SHD_CLIENT_REGISTER;
      memcpy(&cli->peer, &ini->ini_peer, sizeof(shpeer_t));
     break;
    case 3:
    case 4:
      cli->flags &= ~SHD_CLIENT_AUTH;
      break;
    case 5:
    case 6:
      /* ledger notification */
      process_init_ledger_notify(cli, ini);
    case 7:
    case 8:
      /* app notification */
      process_init_app_notify(cli, ini);
      break;
    case 9:
    case 10:
fprintf(stderr, "DEBUG: ini->ini_time diff %f\n", shtimef(stamp) - shtimef(ini->ini_stamp));
      /* time sync */
/* TX_METRIC .. time */
      break;
    default:
      /* term init sequence */
      return (0);
  }

  sched_tx_sink(shpeer_kpriv(&cli->peer), ini, sizeof(tx_init_t));

  return (0);
}
#endif



int txop_init_init(shpeer_t *cli_peer, tx_init_t *ini)
{

  if (!ini)
    return (SHERR_INVAL);

  ini->ini_stamp = shtime(); /* set to cli_peer->last_conn_stamp */

  return (0);
}

int txop_init_confirm(shpeer_t *cli_peer, tx_init_t *ini)
{
#if 0
  shkey_t *key;
  char hash[MAX_SHARE_HASH_LENGTH];
  int ok;

  key = INIT_TX_KEY(ini);
  if (!key)
    return (SHERR_NOMEM);

  memset(hash, 0, sizeof(hash));
  strncpy(hash, shkey_print(key), sizeof(hash) - 1); 
  ok = (0 == strcmp(hash, ini->ini_hash));
  shkey_free(&key);
  if (!ok)
    return (SHERR_ACCESS);
#endif

  return (0);
}


int txop_init_recv(shpeer_t *cli_peer, tx_init_t *ini)
{
  tx_subscribe_t sub;
  shd_t *cli;
  shtime_t stamp;
  shkey_t kpriv;
  int err;


  ini->ini_seq++;

  err = prep_init_tx(ini);
fprintf(stderr, "DEBUG: process_init_tx: %d = prep_init_tx()\n", err);
  if (err)
    return (err);

  memcpy(&kpriv, shpeer_kpriv(cli_peer), sizeof(kpriv));
  cli = sharedaemon_client_find(&kpriv);

if (cli && ini->ini_seq <= cli->cli.net.ini_seq) {
fprintf(stderr, "DEBUG: cli {%x} ini_seq dup (%d)\n", ini->ini_seq);
  return (0);
}


if (ini->ini_seq > 2 && !cli) {
fprintf(stderr, "DEBUG: txinit_recv: ini->ini_seq(%d) && !cli\n", ini->ini_seq);
return (SHERR_INVAL);
}

if (cli)
  cli->cli.net.ini_seq = ini->ini_seq;


fprintf(stderr, "DEBUG: cli {%x} ini_seq %d\n", cli, ini->ini_seq);

  stamp = ini->ini_stamp;
  switch (ini->ini_seq) {
    case 1:
      /* public peer notification */
      if (cli) {
        return (SHERR_NOTUNIQ);
      }

      for (cli = sharedaemon_client_list; cli; cli = cli->next) {
        if ((cli->flags & SHD_CLIENT_NET) &&
            shkey_cmp(&kpriv, shpeer_kpriv(&cli->peer)))
          break;
      }
      if (!cli)
        return (SHERR_INVAL);

      /* establish broadcast channels */
      cli->flags |= SHD_CLIENT_REGISTER;
      memcpy(&cli->peer, &ini->ini_peer, sizeof(shpeer_t));
     break;
    case 2:
      /* establish broadcast channels */
      cli->flags |= SHD_CLIENT_REGISTER;
      memcpy(&cli->peer, &ini->ini_peer, sizeof(shpeer_t));
      break;
    case 3:
    case 4:
      cli->flags &= ~SHD_CLIENT_AUTH;
      break;
    case 5:
    case 6:
      /* default subscription to "shared" public fs partition */
      memset(&sub, 0, sizeof(sub));
      inittx_subscribe(&sub, 
          shpeer_kpub(sharedaemon_peer()), TX_FILE, SHOP_LISTEN);
      sched_tx_sink(shpeer_kpriv(&cli->peer), &sub, sizeof(sub));
#if DEBUG_TODO
      /* ledger notification */
      process_init_ledger_notify(cli, ini);
#endif
      break;
    case 7:
    case 8:
      /* app notification */
      process_init_app_notify(cli, ini);
      break;
    default:
      /* term init sequence */
      return (0);
  }

  sched_tx_sink(shpeer_kpriv(cli_peer), ini, sizeof(tx_init_t));

  return (0);
}

int txop_init_send(shpeer_t *cli_peer, tx_init_t *ini)
{
  return (0);
}


