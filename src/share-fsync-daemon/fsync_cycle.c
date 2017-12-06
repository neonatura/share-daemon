
/*
 *  Copyright 2014 Neo Natura 
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

#include "fsync.h"

int run_state = RUN_NONE;

static double _scan_stamp;

void fsync_cycle_verify(void)
{
  double now;

  now = shtimef(shtime());

  if ((now - _scan_stamp) > PUB_SCAN_WAIT_TIME) {
    run_state = RUN_SCAN;
    _scan_stamp = now;
  }

}

void fsync_cycle_init(void)
{
  fsync_user_init();
}

void fsync_cycle_free(void)
{
  fsync_user_free();
}

void fsync_cycle(void)
{
  struct timeval tv;
  double start_t, end_t;
  int diff;

  run_state = RUN_INIT;
  while (run_state != RUN_NONE) {
    start_t = shtimef(shtime());
    switch (run_state) {
      case RUN_IDLE:
        fsync_cycle_verify();
        break;
      case RUN_INIT:
        fsync_cycle_init();
        run_state = RUN_IDLE;
        break;
      case RUN_SCAN:
        fsync_user_scan();
        run_state = RUN_IDLE;
        break;
    }
    end_t = shtimef(shtime());

    /* wait remainder of 100ms */
    diff = 100 - (end_t - start_t);
    if (!diff)
      continue;

    memset(&tv, 0, sizeof(tv));
    tv.tv_usec = MAX(10, diff) * 1000; /* usec */
    select(0, NULL, NULL, NULL, &tv);
  }

}

