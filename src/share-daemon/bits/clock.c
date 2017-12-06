
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



int inittx_clock(tx_clock_t *clock, shpeer_t *clock_peer)
{
  int err;

  err = tx_init(clock_peer, (tx_t *)clock, TX_CLOCK);
  if (err)
    return (err);

  return (0);
}

tx_clock_t *alloc_clock(shpeer_t *clock_peer)
{
  tx_clock_t *clock;
  int err;

  clock = (tx_clock_t *)calloc(1, sizeof(tx_clock_t));
  if (!clock)
    return (NULL);

  err = inittx_clock(clock, clock_peer);
  if (err)
    return (NULL);

  return (clock);
}


int txop_clock_init(shpeer_t *cli_peer, tx_clock_t *clock)
{
  shd_t *cli;

  if (!cli_peer)
    return (SHERR_INVAL);

  cli = sharedaemon_client_find(shpeer_kpriv(cli_peer));
  if (!cli)
    return (SHERR_NONET);

  clock->clo_send = shtime();
  clock->clo_recv = SHTIME_UNDEFINED;
  shnum_set(0.0, &clock->clo_off);
  shnum_set((shnum_t)MAXDISPERSE, &clock->clo_disp);
  shnum_set(get_sys_precision(), &clock->clo_prec);
  memcpy(&clock->clo_peer, shpeer_kpriv(cli_peer), sizeof(clock->clo_peer));

  return (0);
}

int txop_clock_confirm(shpeer_t *peer, tx_clock_t *clock)
{

  if (clock->clo_send == SHTIME_UNDEFINED)
    return (SHERR_INVAL);

  return (0);
}

void txclock_recv_sync(shd_t *cli, tx_clock_t *clock)
{

  /* calculate offset */
#ifdef REFCLOCK
  refclock_recv_clock(&cli->cli.net.clock, clock);
#endif

  cli->cli.net.clock_stamp = shtime_adj(shtime(), 30);

}

int txop_clock_recv(shpeer_t *peer, tx_clock_t *clock)
{
  shd_t *cli;
  int err;

  if (clock->clo_recv == SHTIME_UNDEFINED) {
    clock->clo_recv = shtime();
    return (tx_send(peer, (tx_t *)clock));
  }

  cli = sharedaemon_client_find(shpeer_kpriv(peer));
  if (cli)
    txclock_recv_sync(cli, clock);

  return (0);
}


int txop_clock_send(shpeer_t *peer, tx_clock_t *clock)
{
  return (0);
}




