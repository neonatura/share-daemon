
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




int api_user_add(shjson_t *reply, shjson_t *param, shmap_t *sess)
{
  shjson_t *result;
  shkey_t *sess_key;
char token[256];
  char *username;
  char *password;
  int err;

  username = shjson_array_str(param, NULL, 0);
  password = shjson_array_str(param, NULL, 0);

  err = shuser_create(username, &sess_key);
  if (err)
    return (err);

  err = shuser_pass_set(username, sess_key, password);
  shkey_free(&sess);
  if (err)
    return (err);

  result = shjson_obj_add(reply, "result");
  shjson_str_add(result, "username", username);

  memset(token, 0, sizeof(token));
  strncpy(token, shkey_print(sess_key), sizeof(token) - 1);
  shmap_set_astr(sess, ashkey_str("token"), token);
  shmap_set_astr(sess, ashkey_str("login"), token);


//  shjson_str_add(result, "ident", shkey_print(id_key));
  //shkey_free(&id_key);

  return (0);
}

