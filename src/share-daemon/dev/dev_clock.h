
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

#ifndef __DEV__NTP_CLOCK_H__ 
#define __DEV__NTP_CLOCK_H__ 

/** overload, clock is free running */
#define  LEAP_NOTINSYNC  0x3 

#define PEER_EVENT  0x080 /* this is a peer event */

#define PEVNT_REACH (4 | PEER_EVENT) /* reachable */

#define MAXDISPERSE  16.0 /* max dispersion */

#define STRATUM   5 /* default stratum */


/*
 * Structure for returning clock status
 */
struct refclockstat {
  uint8_t  type;   /* clock type */
  uint8_t  flags;    /* clock flags */
  uint8_t  haveflags;  /* bit array of valid flags */
  uint16_t lencode;  /* length of last timecode */
  const char *p_lastcode; /* last timecode received */
  uint32_t polls;    /* transmit polls */
  uint32_t noresponse; /* no response to poll */
  uint32_t badformat;  /* bad format timecode received */
  uint32_t baddata;  /* invalid data timecode received */
  uint32_t timereset;  /* driver resets */
  const char *clockdesc;  /* ASCII description */
  double  fudgetime1; /* configure fudge time1 */
  double  fudgetime2; /* configure fudge time2 */
  int fudgeval1;  /* configure fudge value1 */
  uint32_t fudgeval2;  /* configure fudge value2 */
  uint8_t  currentstatus;  /* clock status */
  uint8_t  lastevent;  /* last exception event */
  uint8_t  leap;   /* leap bits */
  struct  ctl_var *kv_list; /* additional variables */
};


struct refclockio 
{
  struct  refclockio *next; /* link to next structure */
#if 0
  void  (*clock_recv) (struct recvbuf *); /* completion routine */
  int   (*io_input)   (struct recvbuf *); /* input routine -
        to avoid excessive buffer use
        due to small bursts
        of refclock input data */
#endif
  struct peer *srcclock;  /* refclock peer */
  int datalen;  /* length of data */
  int fd;   /* file descriptor */
  u_long  recvcount;  /* count of receive completions */
  int active;   /* nonzero when in use */
};



/** seconds since startup */
extern u_long current_time;

/** system leap indicator */
extern u_char sys_leap;

/** system stratum */
extern u_char sys_stratum;

/** local clock precision */
extern char sys_precision;

extern struct refclockproc refclock_dummy_proc;


void refclock_receive(devclock_t *peer);

/**
 * report_event - report an event to the trappers
 * @param err error code
 * @param peer peer structure
 * @param str protostats string
 */
void report_event(int err, devclock_t *peer, const char *str);

void get_systime(l_fp *now);

/*
 * clock_filter - add incoming clock sample to filter register and run
 *		  the filter procedure to find the best sample.
 */
void
clock_filter(
	devclock_t *peer,		/* peer structure pointer */
	double	sample_offset,		/* clock offset */
	double	sample_delay,		/* roundtrip delay */
	double	sample_disp		/* dispersion */
	);


void refclock_recv_clock(devclock_t *peer, tx_clock_t *tx);

void refclock_init(devclock_t *peer, struct refclockproc *proc);

shnum_t get_sys_precision(void);


#endif /* ndef __DEV__NTP_CLOCK_H__ */
