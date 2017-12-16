
/*
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
 */  

#include "sharedaemon.h"
#include <stdio.h>

char *oauth_api_token(shd_t *api_cli, shmap_t *sess)
{
  static char ret_buf[MAX_SHARE_HASH_LENGTH];
  char *token;
  char *login;
  int err;

  token = shmap_get_str(sess, ashkey_str("access_token"));
  if (!token) {
    shkey_t *cli_k = shkey_gen(oauth_sess_token(sess));
    shkey_t *serv_k = oauth_sess_id(api_cli);
    shkey_t *key;

    key = shkey_xor(cli_k, serv_k);
    shkey_free(&cli_k);
    shkey_free(&serv_k);

    token = shkey_print(key);
    shmap_set_astr(sess, "access_token", token);
    shkey_free(&key);
  }

  memset(ret_buf, 0, sizeof(ret_buf));
  strncpy(ret_buf, token, sizeof(ret_buf)-1);

  return (ret_buf);
}


