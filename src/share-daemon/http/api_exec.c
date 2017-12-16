
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

#define API_USER_ADD "user.add"
#define API_MEDIA_LIST "media.list"
#define API_MEDIA_READ "media.read"
#define API_MEDIA_WRITE "media.write"
#define API_CRYPT_READ "crypt.read"
#define API_CRYPT_WRITE "crypt.write"
#define API_GEO_SET "geo.set"
#define API_GEO_PLACE "geo.place"
#define API_GEO_IPADDR "geo.ipaddr"
#define API_GEO_SCAN "geo.scan"

int api_authorize(shd_t *cli, char *api_key, shmap_t *sess)
{
  char *token;

  if (!sess) {
fprintf(stderr, "DEBUG: api_authorize; no session\n");
    return (SHERR_ACCESS);
}

  token = oauth_api_token(cli, sess);
  if (!token) {
fprintf(stderr, "DEBUG: api_authorize; no oauth_api_token\n");
return (SHERR_ACCESS);
}
  if (0 != strcmp(api_key, token)) {
fprintf(stderr, "DEBUG: ERROR: ACCESS-TOKEN: cmp_key'%s', api_key'%s'\n", token, api_key);
    return (SHERR_ACCESS);
  }

  return (0);
}

int api_exec(shd_t *cli, char *api_key, char *reqid, char *method, shjson_t *param, shmap_t *sess)
{
  shjson_t *reply;
  char *text;
  int ret_err;

  reply = shjson_init(NULL);
  shjson_num_add(reply, "id", strtol(reqid?reqid:"", NULL, 0));

fprintf(stderr, "DEBUG: api_exec: method \"%s\"\n", method);

  ret_err = 0;
  if (0 != api_authorize(cli, api_key, sess)) {
    ret_err = SHERR_ACCESS;
  } else if (!method) {
    ret_err = SHERR_INVAL;
  } else if (0 == strcmp(method, API_USER_ADD)) {
    ret_err = api_user_add(reply, param, sess);
  } else if (0 == strcmp(method, API_MEDIA_LIST)) {
    ret_err = api_media_list(reply, param, sess);
  } else if (0 == strcmp(method, API_MEDIA_READ)) {
    ret_err = api_media_read(reply, param, sess);
  } else if (0 == strcmp(method, API_MEDIA_WRITE)) {
    ret_err = api_media_write(reply, param, sess);
  } else if (0 == strcmp(method, API_CRYPT_READ)) {
    ret_err = api_crypt_read(reply, param, sess);
  } else if (0 == strcmp(method, API_CRYPT_WRITE)) {
    ret_err = api_crypt_write(reply, param, sess);
  } else if (0 == strcmp(method, API_GEO_SET)) {
    ret_err = api_geo_set(reply, param, sess);
  } else if (0 == strcmp(method, API_GEO_PLACE)) {
    ret_err = api_geo_place(reply, param, sess);
  } else if (0 == strcmp(method, API_GEO_IPADDR)) {
    ret_err = api_geo_ipaddr(reply, param, sess);
  } else if (0 == strcmp(method, API_GEO_SCAN)) {
    ret_err = api_geo_scan(reply, param, sess);
  } 

  if (ret_err) {
    shjson_t *error = shjson_array_add(reply, "error");
    shjson_num_add(error, NULL, ret_err);
    shjson_str_add(error, NULL, sherrstr(ret_err));
    shjson_null_add(error, NULL);

    shjson_null_add(reply, "result");
  } else {
    shjson_null_add(reply, "error");
  }
    

  text = shjson_print(reply);
  oauth_html_template(cli->buff_out, text);
  free(text);

  shjson_free(&reply);
}


