
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

#ifndef __SHAREDAEMON_H__
#define __SHAREDAEMON_H__
/**
 *  @brief Share Transaction Service 
 *  @note The Share Library is hosted at https://github.com/neonatura/share/
 *  @defgroup sharedaemon
 *  @{
 */

#undef __STRICT_ANSI__ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include "share.h"

#include "bits/bits.h"
#include "dev/dev.h"
#include "dev/dev_usb.h"
#include "dev/dev_clock.h"
#include "dev/card_kmap.h"
#include "dev/card_usb.h"
#include "dev/fpga_usb.h"
#include "dev/leitch_serial.h"
#include "dev/clock_local.h"

#include "sharedaemon_file.h"
#include "sharedaemon_client.h"
#include "sharedaemon_device.h"
#include "sharedaemon_server.h"
#include "sharedaemon_app.h"
#include "sharedaemon_store.h"
#include "sharedaemon_bcast.h"
#include "sharedaemon_peer.h"

#include "bits/init.h"
#include "bits/metric.h"

#include "http/oauth.h"
#include "http/oauth_session.h"
#include "http/oauth_response.h"
#include "http/oauth_2fa.h"
#include "http/oauth_db.h"
#include "http/oauth_template.h"
#include "http/oauth_admin.h"

#include "http/api_exec.h"
#include "http/api_media.h"
#include "http/api_geo.h"
#include "http/api_crypt.h"

#define SHARE_DAEMON_PORT 32080
#define SHARE_HTTP_DAEMON_PORT 32079

/**
 * The maximum number of additional gateway ports allowed.
 * @note This effictively limits the share daemon's maximum number of network clients to 32768 connections [at 8192 max file descriptors per process].
 */
#define MAX_SHARE_GATEWAY_PORTS 4

#define SHARE_GATEWAY_PORT(_idx) \
  ((_idx < 0 || _idx >= MAX_SHARE_GATEWAY_PORTS) ? 0 : \
   SHARE_DAEMON_PORT + (_idx) + 1)

/** server run modes */
#define PROC_SERVE "serve"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#ifndef MIN
#define MIN(a,b) \
  (a < b ? a : b)
#endif

#ifndef MAX
#define MAX(a,b) \
  (a > b ? a : b)
#endif

#define sharedaemon_peer() \
  server_peer

extern shpeer_t *server_peer;
extern tx_account_t *server_account;
extern tx_ledger_t *server_ledger;
extern int server_port;

/*
extern char process_path[PATH_MAX + 1];
extern char process_file_path[PATH_MAX + 1];
extern char process_socket_host[PATH_MAX + 1];
extern unsigned int process_socket_port;
*/

/**
 *  @}
 */

/**
 *  @mainpage The Share Daemon
 *
 *  <h3>The Share Daemon reference manual.</h3>
 *
 *  This documentation covers the Share Daemon provided by the Share library. 
 *
 *  The Share Daemon is broken down into the following sub-systems:
 *
 *  - \subpage sharedaemon_device "External Device Communication"
 *  <dl>The Share Daemon provides the capability to communicate with common usb, serial, and network-based external devices.</dl>
 */

/**
 * @page sharedaemon_device External Device Communicaton
 *
 * The Share Daemon utilizes external devices for various purposes. 
 *
 * <u>Card Readers</u>
 *
 * The Share Daemon utilizes card readers in order to generate biometric authentication tokens that can be used to verify a user by varying levels.
 *
 *
 * Supported Devices:
 * - <a href="https://www.magtek.com/shop/mini.aspx">MagTek Mini Swipe Card Reader</a> (USB)
 *
 * References:
 * - \ref sharedaemon_devicecard Card Device Documentation
 *
 */

#endif /* __SHAREDAEMON_H__ */

