

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

#define __BITS__EVENT_C__


int inittx_event(tx_event_t *event, shgeo_t *geo, shtime_t stamp, shkey_t *ctx_key)
{
  int err;

  if (stamp == SHTIME_UNDEFINED)
    stamp = shtime();

  event->eve_stamp = stamp;
  memcpy(&event->eve_geo, geo, sizeof(event->eve_geo));
  memcpy(&event->eve_peer, sharedaemon_peer(), sizeof(event->eve_peer));
  if (ctx_key)
    memcpy(&event->eve_ctx, ctx_key, sizeof(event->eve_ctx));

  err = tx_init(NULL, event, TX_EVENT);
  if (err)
    return (err);

  return (0);
}

tx_event_t *alloc_event(shgeo_t *geo, shtime_t stamp, shkey_t *ctx_key)
{
  tx_event_t *event;
  int err;

  event = (tx_event_t *)calloc(1, sizeof(tx_event_t));
  if (!event)
    return (NULL);

  err = inittx_event(event, geo, stamp, ctx_key);
  if (err) {
    free(event);
    return (err);
  }

  return (event);
}

int txop_event_init(shpeer_t *cli_peer, tx_event_t *event)
{
  shtime_t now;

  now = shtime();
  if (event->eve_stamp == SHTIME_UNDEFINED ||
      shtime_before(event->eve_stamp, now))
    event->eve_stamp = now; /* >= current time */

  return (0);
}

int txop_event_confirm(shpeer_t *cli_peer, tx_event_t *event)
{
  int err;


  return (0);
}
int txop_event_send(shpeer_t *cli_peer, tx_event_t *event)
{
  return (0);
}
int txop_event_recv(shpeer_t *cli_peer, tx_event_t *event)
{
  return (0);
}
