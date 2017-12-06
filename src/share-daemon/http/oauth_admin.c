
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

void oauth_admin_redir_login(shd_t *cli, char *client_id)
{
  char *enc_cid = http_token_encode(client_id);
  char *enc_uri = http_token_encode("/admin");
  char buf[1024];

  sprintf(buf,
      "HTTP/1.1 302 Found\r\n"
      "Location: /auth?response_type=token&client_id=%s&redirect_uri=%s\r\n"
      "Content-Length: 0\r\n"
      "\r\n",
      enc_cid, enc_uri);
  shbuf_catstr(cli->buff_out, buf);

  free(enc_cid);
  free(enc_uri);
}

static int oauth_admin_verify_fullname(char *text)
{

  if (strlen(text) < 5)
    return (FALSE);

  /* verify name contains a space or comma character */
  return ((strchr(text, ' ') || strchr(text, ',')) ? TRUE : FALSE);
}

static int oauth_admin_verify_address(char *text)
{

  if (strlen(text) < 4)
    return (FALSE);

  if (*text == '@')
    return (TRUE); /* lat/lon */
  if (tolower(*text) == 'p')
    return (TRUE); /* PO BOX */

  /* verify address contains a street number */
  return (isdigit(*text));
}

static int oauth_admin_verify_zipcode(char *text)
{

  if (strlen(text) < 5)
    return (FALSE);

  return (isdigit(*text));
}

static int oauth_admin_verify_phone(char *text)
{

  if (*text == '+')
    text++; /* iterate past country code symbol */

  if (strlen(text) < 10)
    return (FALSE);

  /* ensure prefix is numeric */
  return (isdigit(*text));
}

int oauth_admin_api_user(shd_t *cli, char *client_id, char *password, char *fullname, char *address, char *zipcode, char *phone, int b_2fa)
{
  shmap_t *sess;
  char buf[1024];
  char warning[256];
  int err;

  if (!client_id)
    client_id = "";

  sess = oauth_sess_load(cli, client_id);
  if (!sess)
    return (SHERR_INVAL);

  if (!oauth_sess_login(sess)) {
    oauth_admin_redir_login(cli, client_id);
    return (0);
  }

  /* apply new user-defined settings */
  if (fullname && *fullname) {
    if (!oauth_admin_verify_fullname(fullname))
      strcpy(warning, "Please specify a valid 'Real Name'.");
    else
      shmap_set_astr(sess, ashkey_str("fullname"), fullname);
  }
  if (address && *address) {
    if (!oauth_admin_verify_address(address))
      strcpy(warning, "Please specify a valid 'Street Address'.");
    else
      shmap_set_astr(sess, ashkey_str("address"), address);
  }
  if (zipcode && *zipcode) {
    if (!oauth_admin_verify_zipcode(zipcode))
      strcpy(warning, "Please specify a valid 'Zip Code'.");
    else
      shmap_set_astr(sess, ashkey_str("zipcode"), zipcode);
  }
  if (phone && *phone) {
    if (!oauth_admin_verify_phone(phone))
      strcpy(warning, "Please specify a valid 'Phone Number'.");
    else
      shmap_set_astr(sess, ashkey_str("phone"), phone);
  }

  /* initialize variables */
  if (!shmap_get_str(sess, ashkey_str("fullname")))
    shmap_set_astr(sess, ashkey_str("fullname"), "");
  if (!shmap_get_str(sess, ashkey_str("address")))
    shmap_set_astr(sess, ashkey_str("address"), "");
  if (!shmap_get_str(sess, ashkey_str("zipcode")))
    shmap_set_astr(sess, ashkey_str("zipcode"), "");
  if (!shmap_get_str(sess, ashkey_str("2fa")))
    shmap_set_astr(sess, ashkey_str("2fa"), "0");

  /* response with JSON context */
  shjson_t *json = shjson_init(NULL);

  /* core attributes */
  shjson_str_add(json, "fullname", 
      shmap_get_str(sess, ashkey_str("fullname")));
  shjson_str_add(json, "address", 
      shmap_get_str(sess, ashkey_str("address")));
  shjson_str_add(json, "zipcode", 
      shmap_get_str(sess, ashkey_str("zipcode")));
  shjson_str_add(json, "phone", 
      shmap_get_str(sess, ashkey_str("phone")));
  shjson_num_add(json, "2fa", 
      atoi(shmap_get_str(sess, ashkey_str("2fa"))));

  oauth_html_json_template(cli->buff_out, json);
  shjson_free(&json);

  return (0);
}

int oauth_admin_api_client(shd_t *cli, char *client_id, char *title, char *logo_url)
{
  shmap_t *sess;
  char buf[1024];
  int err;

  if (!client_id)
    client_id = "";

  sess = oauth_sess_load(cli, client_id);
  if (!sess)
    return (SHERR_INVAL);

  if (!oauth_sess_login(sess)) {
    oauth_admin_redir_login(cli, client_id);
    return (0);
  }

  if (title && logo_url && (*title || *logo_url)) {
    oauth_sess_client_set(sess, client_id, title, logo_url);
  }

  sprintf(buf, "<html><form action=\"/admin\"><input name=\"client_id\" value=\"%s\" disabled></input><input name=\"title\" value=\"%s\"></input><input name=\"logo_url\" value=\"%s\"><input type=\"submit\"></input></form></html>\r\n", client_id, title?title:"", logo_url?logo_url:"");
  oauth_html_template(cli->buff_out, buf);

  return (0);

}

int oauth_admin_user(shd_t *cli, char *client_id, char *password, char *fullname, char *address, char *zipcode, char *phone, int b_2fa)
{
  shmap_t *sess;
  char buf[1024];
  char warning[1024];
  int err;

  memset(warning, 0, sizeof(warning));

  if (!client_id)
    client_id = "";

  sess = oauth_sess_load(cli, client_id);
  if (!sess)
    return (SHERR_INVAL);

  if (!oauth_sess_login(sess)) {
    oauth_admin_redir_login(cli, client_id);
    return (0);
  }

  /* update attributes with user-defined settings */
  if (fullname && *fullname) {
    if (!oauth_admin_verify_fullname(fullname))
      strcpy(warning, "Please specify a valid 'Real Name'.");
    else
      shmap_set_astr(sess, ashkey_str("fullname"), fullname);
  }
  if (address && *address) {
    if (!oauth_admin_verify_address(address))
      strcpy(warning, "Please specify a valid 'Street Address'.");
    else
      shmap_set_astr(sess, ashkey_str("address"), address);
  }
  if (zipcode && *zipcode) {
    if (!oauth_admin_verify_zipcode(zipcode))
      strcpy(warning, "Please specify a valid 'Zip Code'.");
    else
      shmap_set_astr(sess, ashkey_str("zipcode"), zipcode);
  }
  if (phone && *phone) {
    if (!oauth_admin_verify_phone(phone))
      strcpy(warning, "Please specify a valid 'Phone Number'.");
    else
      shmap_set_astr(sess, ashkey_str("phone"), phone);
  }

  /* initialize variables */
  if (!shmap_get_str(sess, ashkey_str("fullname")))
    shmap_set_astr(sess, ashkey_str("fullname"), "");
  if (!shmap_get_str(sess, ashkey_str("address")))
    shmap_set_astr(sess, ashkey_str("address"), "");
  if (!shmap_get_str(sess, ashkey_str("zipcode")))
    shmap_set_astr(sess, ashkey_str("zipcode"), "");
  if (!shmap_get_str(sess, ashkey_str("2fa")))
    shmap_set_astr(sess, ashkey_str("2fa"), "0");

  oauth_admin_user_template(sess, cli->buff_out, client_id, warning);

  return (0);
}

