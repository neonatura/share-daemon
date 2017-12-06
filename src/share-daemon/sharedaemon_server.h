
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

#ifndef __SHAREDAEMON_SERVER_H__
#define __SHAREDAEMON_SERVER_H__

#define MAX_CLIENT_CONNECTIONS 16
#define MAX_CLIENT_CONNECTION_IDLE_TIME 1.0
#define MAX_CLIENT_IDLE_TIME 600 /* ten minutes */


extern int listen_sk;
extern int http_listen_sk;


void sharedaemon_server(char *subcmd);

void cycle_main(int run_state);

/**
 * Broadcast a transaction to all peers.
 * @note The broadcast is suppressed for the originating source application.
 */
void broadcast_raw(void *raw_data, size_t data_len);

void client_http_tokens(char *tmpl, shmap_t *fields);

#endif /* __SHAREDAEMON_SERVER_H__ */

