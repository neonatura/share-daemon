
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura
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

#ifndef __CARD__DEV_H__
#define __CARD__DEV_H__

#ifdef USE_USB
#include <libusb.h>
#endif

#include "ntp_fp.h"

/** A usb device. */
#define SHDEV_USB (1 << 0)
/** A serial device. */
#define SHDEV_SERIAL (1 << 1)
/** A network-enabled device. */
#define SHDEV_NET (1 << 2)
/** Automatically start device upon initialization. */
#define SHDEV_START (1 << 3)
/** A card reader. */
#define SHDEV_CARD (1 << 4)
/** A time synchronization device. */
#define SHDEV_CLOCK (1 << 5)

#define DEF_MAGTEK 0
#define DEF_ZTEX 1
#define DEF_LEITCH 2

#define USB_MAGTEK_DEVICE (&device_def[DEF_MAGTEK])
#define USB_ZTEX_DEVICE (&device_def[DEF_ZTEX])
#define USB_LEITCH_DEVICE (&device_def[DEF_LEITCH])


#define BASE_CARD_SIZE (sizeof(uint16_t) + sizeof(shtime_t) + sizeof(shkey_t))

#define CARDTYPE_VISA "VIS"
#define CARDTYPE_MASTER "MC"
#define CARDTYPE_NEONATURA "NEO"
#define CARDTYPE_AMEX "AME"
#define CARDTYPE_DISCOVER "DIS"
#define CARDTYPE_MAESTRO "MAE"
#define CARDTYPE_AUX "AUX"

/* card service code - first digit */
#define SHCARD_INTERNATIONAL (1 << 0) /* '1' / '2' */ 
#define SHCARD_NATIONAL (1 << 1) /* '5' / '6' */
#define SHCARD_CHIP (1 << 2) /* '2' / '6' */
/* card service code - second digit */
#define SHCARD_ONLINE (1 << 6) /* '2' */
#define SHCARD_ONLINE_BILATERAL (1 << 7) /* '4' */
/* card service code - third digit */
#define SHCARD_PIN (1 << 10) /* '0' */
#define SHCARD_SERVICE (1 << 11) /* '2' no cash / '5' pin */
#define SHCARD_ATM (1 << 12) /* '3' ATM */
#define SHCARD_CASH (1 << 13) /* '4' CASH */
#define SHCARD_PIN_PREF (1 << 14) /* '6' pin preferred / '7' +SERVICE */

/** Determines whether card flags have no service restrictions. */
#define IS_SHCARD_NO_RESTRICTION(_flags) \
  (!(_flags & SHCARD_PIN) && \
   !(_flags & SHCARD_SERVICE) && \
   !(_flags & SHCARD_ATM) && \
   !(_flags & SHCARD_CASH) && \
   !(_flags & SHCARD_PIN_PREF))




#ifdef USE_USB
typedef libusb_device_handle shdev_usb_t;
#else
typedef void shdev_usb_t;
#endif


/**
 * A card-swiping device is used in order to generate a <i>Metric Transaction</i>. A Metric Transaction is broadcasted to all local processes listing to TX_METRIC transactions.
 * @ingroup sharedaemon_device
 * @defgroup sharedaemon_devicecard
 */

struct shcard_t
{
  /** A three character gateway identifier */
  char card_type[4]; 

  /** The card's 'service code' as a bitvector. */
  uint32_t card_flags;

  /** The card owner's account id. */
  uint64_t card_acc;

  /** The card's expiration date. */
  shtime_t card_expire;

  /** A key reference to the card's 12-19 digit ID number. */
  uint64_t card_id;

  /** Peer information for a 'NAT' type card's issuer. */
  shpeer_t card_issuer;
};
typedef struct shcard_t shcard_t;

#include "card_kmap.h"
#include "card_csc.h"

/**
 * @}
 */


struct refclockproc {
  void *  unitptr;  /* pointer to unit structure */
//  struct refclock * conf; /* refclock_conf[type] */
//  struct refclockio io; /* I/O handler structure */
  u_char  leap;   /* leap/synchronization code */
  u_char  currentstatus;  /* clock status */
  u_char  lastevent;  /* last exception event */
  u_char  type;   /* clock type */
  const char *clockdesc;  /* clock description */
  u_long  nextaction; /* local activity timeout */
//  void  (*action)(devclock_t *); /* timeout callback */

#define BMAX    128 /* max timecode length */
  char  a_lastcode[BMAX]; /* last timecode received */
  int lencode;  /* length of last timecode */

  int year;   /* year of eternity */
  int day;    /* day of year */
  int hour;   /* hour of day */
  int minute;   /* minute of hour */
  int second;   /* second of minute */
  long  nsec;   /* nanosecond of second */
  u_long  yearstart;  /* beginning of year */
  int coderecv; /* put pointer */
  int codeproc; /* get pointer */
  shtime_t  lastref;  /* reference timestamp */
  shtime_t  lastrec;  /* receive timestamp */
  double  offset;   /* mean offset */
  double  disp;   /* sample dispersion (precision) */
  double  jitter;   /* jitter (mean squares) */
#define MAXSTAGE  60  /* max median filter stages  */
  double  filter[MAXSTAGE]; /* median filter */
  double delay; /* rtt/2 */

  /*
 *    * Configuration data
 *       */
  double  fudgetime1; /* fudge time1 */
  double  fudgetime2; /* fudge time2 */
  u_char  stratum;  /* server stratum */
  uint32_t refid;    /* reference identifier */
  u_char  sloppyclockflag; /* fudge flags */

  /*
 *    * Status tallies
 *       */
  u_long  timestarted;  /* time we started this */
  u_long  polls;    /* polls sent */
  u_long  noreply;  /* no replies to polls */
  u_long  badformat;  /* bad format reply */
  u_long  baddata;  /* bad data reply */
};

struct devclock_t 
{
  /* device */
  struct refclockproc *procptr; /* refclock structure pointer */
  u_char  refclktype; /* reference clock type */
  u_char  refclkunit; /* reference clock unit number */
  u_char  sstclktype; /* clock type for system status word */

  /* ntp packet */
  u_char  leap;   /* local leap indicator */
  u_char  pmode;    /* remote association mode */
  u_char  stratum;  /* remote stratum */
  u_char  ppoll;    /* remote poll interval */
  char  precision;  /* remote clock precision */
  double  rootdelay;  /* roundtrip delay to primary source */
  double  rootdisp; /* dispersion to primary source */
  uint32_t refid;    /* remote reference ID */
  shtime_t  reftime;  /* update epoch */

  /*  Ephemeral state */
  shtime_t  aorg;   /* origin timestamp */
  u_char  reach;    /* reachability register */
  shtime_t  dst;    /* destination timestamp */
 
  /* stats */
  u_long  timereset;  /* time stat counters were reset */
  u_long  timereceived; /* last packet received time */
  u_long  timereachable;  /* last reachable/unreachable time */
  u_long  sent;   /* packets sent */
  u_long  received; /* packets received */
  u_long  processed;  /* packets processed */
  u_long  badauth;  /* bad authentication (TEST5) */
  u_long  bogusorg; /* bogus origin (TEST2, TEST3) */
  u_long  oldpkt;   /* old duplicate (TEST1) */
  u_long  seldisptoolarge; /* bad header (TEST6, TEST7) */
  u_long  selbroken;  /* KoD received */

};
typedef struct devclock_t devclock_t;

/**    
 * Structure for returnin device status
 */   
struct shdev_ctrl_t
{
  unsigned char  type;   /* clock type */
  unsigned char  flags;    /* clock flags */
  unsigned char  haveflags;  /* bit array of valid flags */
  unsigned short lencode;  /* length of last timecode */
  const char *p_lastcode; /* last timecode received */
  uint32_t polls;    /* transmit polls */
  uint32_t noresponse; /* no response to poll */
  uint32_t badformat;  /* bad format timecode received */
  uint32_t baddata;  /* invalid data timecode received */
  uint32_t timereset;  /* driver resets */
  const char *clockdesc;  /* ASCII description */
  double  fudgetime1; /* configure fudge time1 */
  double  fudgetime2; /* configure fudge time2 */
  int32_t fudgeval1;  /* configure fudge value1 */
  uint32_t fudgeval2;  /* configure fudge value2 */
  unsigned char  currentstatus;  /* clock status */
  unsigned char  lastevent;  /* last exception event */
  unsigned char  leap;   /* leap bits */
  struct  ctl_var *kv_list; /* additional variables */
};
typedef struct shdev_ctrl_t shdev_ctrl_t;


struct shdev_t;
typedef int (*shdev_op_t)(struct shdev_t *);

struct shdev_def_t {
  char *name;
  int arg1;
  int arg2;
  int flags;
  shdev_op_t init;
  shdev_op_t start;
  shdev_op_t poll;
  shdev_op_t control;
  shdev_op_t timer;
  shdev_op_t shutdown;
};
typedef struct shdev_def_t shdev_def_t;

struct shdev_t {
  /** device definition */
  shdev_def_t *def;

  /** error state of device */
  int err_state;

  /** stats/control */
  shdev_ctrl_t ctrl;

  /* usb driver */
  shdev_usb_t *usb;
  int iface;
  int index;

  /* content */
  union {
    shcard_t card;
    devclock_t clock;
  } data;

  /* io stream */
  shbuf_t *buff;

  struct shdev_t *next;
};
typedef struct shdev_t shdev_t;




#endif /* ndef __CARD__DEV_H__ */
