
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

#define __OAUTH_TEMPLATE_C__

#include "sharedaemon.h"

void oauth_html_template(shbuf_t *buff, char *text)
{
  size_t len = strlen(text);
  char key_str[MAX_SHARE_HASH_LENGTH];
  char buf[1024];
  time_t now;

  /* http */
  strcpy(buf, "HTTP/1.1 200 OK\r\n");
  shbuf_catstr(buff, buf);

  /* timestamp */
  now = time(NULL);
  strftime(buf, sizeof(buf) - 1, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", gmtime(&now));
  shbuf_catstr(buff, buf);

  /* server */
  sprintf(buf, "Server: %s/%s\r\n", get_libshare_title(), get_libshare_version());
  shbuf_catstr(buff, buf);

  /* length */
  sprintf(buf, "Content-Length: %u\r\n", (unsigned int)strlen(text));
  shbuf_catstr(buff, buf);

  /* mime */
  shbuf_catstr(buff, "Content-Type: text/html; charset=UTF-8\r\n");

  /* cache directive */
  shbuf_catstr(buff, "Cache-Control: private, max-age=0, no-cache\r\n");

  /* terminator */
  shbuf_catstr(buff, "\r\n");

  /* content */
  shbuf_catstr(buff, text);
}

void oauth_html_json_template(shbuf_t *buff, shjson_t *json)
{
  char key_str[MAX_SHARE_HASH_LENGTH];
  char buf[1024];
  time_t now;
  char *text = shjson_print(json);
  size_t len = strlen(text);

  /* http */
  strcpy(buf, "HTTP/1.1 200 OK\r\n");
  shbuf_catstr(buff, buf);

  /* timestamp */
  now = time(NULL);
  strftime(buf, sizeof(buf) - 1, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", gmtime(&now));
  shbuf_catstr(buff, buf);

  /* server */
  sprintf(buf, "Server: %s/%s\r\n", get_libshare_title(), get_libshare_version());
  shbuf_catstr(buff, buf);

  /* length */
  sprintf(buf, "Content-Length: %u\r\n", (unsigned int)strlen(text));
  shbuf_catstr(buff, buf);

  /* mime */
  shbuf_catstr(buff, "Content-Type: application/json; charset=UTF-8\r\n");

  /* cache directive */
  shbuf_catstr(buff, "Cache-Control: private, max-age=0, no-cache\r\n");

  /* terminator */
  shbuf_catstr(buff, "\r\n");

  /* content */
  shbuf_catstr(buff, text);

  free(text);
}

/**
 * Move to next step of authorization.
 */
void oauth_response_token_template(shmap_t *sess, shbuf_t *buff, char *client_id)
{
  char key_str[MAX_SHARE_HASH_LENGTH];
  char text[20480];
  char *enc_scope;
  char *enc_uri;
  char *enc_cid;
  char *str;

  
  enc_uri = http_token_encode(oauth_sess_redirect_url(sess, client_id));
  enc_cid = http_token_encode(client_id);
  enc_scope = http_token_encode(oauth_sess_scope_str(sess, client_id));

  sprintf(text,
      "HTTP/1.1 302 Found\r\n"
      "Location: /auth?response_type=token&client_id=%s&redirect_uri=%s&scope=%s\r\n"
      "Content-Length: 0\r\n"
      "\r\n",
      enc_cid, enc_uri, enc_scope);

  free(enc_uri);
  free(enc_cid);
  free(enc_scope);
fprintf(stderr, "DEBUG: oauth_response_token_template: \"%s\"\n", text); 

  shbuf_catstr(buff, text);
}

/**
 * redirect back to originating site due to successful login
 */
void oauth_response_app_template(shmap_t *sess, shbuf_t *buff, char *client_id)
{
  char key_str[MAX_SHARE_HASH_LENGTH];
  char text[20480];
  char uri[1024];
  char *token;
  char *enc_uri;
  char *str;

  memset(uri, 0, sizeof(uri));
  strncpy(uri, oauth_sess_redirect_url(sess, client_id), sizeof(uri) - 1);

  if (!*uri) {
fprintf(stderr, "DEBUG: oauth_response_app_template: no redirect uri specified by client '%s'.\n", client_id);
    strcpy(uri, "/");
}

  /* successful login. */
  token = http_token_encode(oauth_sess_token(sess));

  if (strchr(uri, '#') || uri[strlen(uri)-1] == '?' || uri[strlen(uri)-1] == '&') 
    snprintf(uri+strlen(uri), sizeof(uri)-strlen(uri)-1, "code=%s", token);
  else if (strchr(uri, '?'))
    snprintf(uri+strlen(uri), sizeof(uri)-strlen(uri)-1, "&code=%s", token);
  else
    snprintf(uri+strlen(uri), sizeof(uri)-strlen(uri)-1, "?code=%s", token);

  free(token);

  sprintf(text,
      "HTTP/1.1 302 Found\r\n"
      "Location: %s\r\n"
      "Content-Length: 0\r\n"
      "\r\n",
      uri);
  shbuf_catstr(buff, text);

fprintf(stderr, "DEBUG: oauth_response_token: \"%s\"\n", text); 

}

char *oauth_response_app_error_url(shmap_t *sess, char *client_id)
{
  static char ret_url[1024];
  char key_str[MAX_SHARE_HASH_LENGTH];
  char *uri;
  char *str;

  memset(ret_url, 0, sizeof(ret_url));

  uri = oauth_sess_redirect_url(sess, client_id);

  strncpy(ret_url, uri, sizeof(ret_url) - 1);
  if (strchr(ret_url, '#') || ret_url[strlen(ret_url)-1] == '?' || ret_url[strlen(ret_url)-1] == '&') 
    snprintf(ret_url+strlen(ret_url), sizeof(ret_url)-strlen(ret_url)-1, "error=access_denied");
  else if (strchr(ret_url, '?'))
    snprintf(ret_url+strlen(ret_url), sizeof(ret_url)-strlen(ret_url)-1, "&error=access_denied");
  else
    snprintf(ret_url+strlen(ret_url), sizeof(ret_url)-strlen(ret_url)-1, "?error=access_denied");

  return (ret_url);
}
void oauth_response_app_error_template(shmap_t *sess, shbuf_t *buff, char *client_id)
{
  char text[2048];

  sprintf(text,
      "HTTP/1.1 302 Found\r\n"
      "Location: %s\r\n"
      "Content-Length: 0\r\n"
      "\r\n",
      oauth_response_app_error_url(sess, client_id));
  shbuf_catstr(buff, text);

fprintf(stderr, "DEBUG: oauth_response_error_template: \"%s\"\n", text); 

}

void oauth_response_notfound_template(shbuf_t *buff)
{
  char text[1024];
  char buf[256];

  strcpy(buf,
    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
    "<html><head>\r\n"
    "<title>404 Not Found</title>\r\n"
    "</head><body>\r\n"
    "<h1>Not Found</h1>\r\n"
    "<p>The requested URL was not found on this server.</p>\r\n"
    "</body></html>\r\n");

  sprintf(text,
    "HTTP/1.1 404 Not Found\r\n"
    "Server: %s/%s\r\n"
    "Content-Length: %d\r\n"
    "Content-Type: text/html; charset=iso-8859-1\r\n"
    "\r\n",
    get_libshare_title(), get_libshare_version(), strlen(buf));

  shbuf_catstr(buff, text);
  shbuf_catstr(buff, buf);

}


void oauth_response_access_template(shmap_t *sess, shbuf_t *buff, char *client_id)
{
  char *app_title = oauth_sess_client_title(sess, client_id);
  char text[20480];
  char key_str[MAX_SHARE_HASH_LENGTH];
  char buf[256];
  char *token;
  char *str;

  token = http_token_encode(oauth_sess_token(sess));

  memset(text, 0, sizeof(text));
  snprintf(text, sizeof(text)-1, _oauth_response_access_html, 
      _neo_natura_logo, oauth_app_header_html(sess, client_id), 
      _person_icon, app_title, client_id, token,
      oauth_response_app_error_url(sess, client_id));

  free(token);

  oauth_html_template(buff, text);
}

void oauth_response_login_template(shmap_t *sess, shbuf_t *buff, char *client_id, char *warning)
{
  char *app_title = oauth_sess_client_title(sess, client_id);
  char text[20480];
  char chk_2fa_on[32];
  char chk_2fa_off[32];
  char *str;

  str = shmap_get_str(sess, ashkey_str("2fa"));
  if (str && 0 == strcmp(str, "1"))
    strcpy(chk_2fa_on, " CHECKED");
  else
    strcpy(chk_2fa_off, " CHECKED");

  memset(text, 0, sizeof(text));
  snprintf(text, sizeof(text)-1, _oauth_response_login_html, 
    _neo_natura_logo, oauth_app_header_html(sess, client_id),
    _person_icon, app_title, 
    warning ? warning : "", client_id,
    chk_2fa_on, chk_2fa_off);

  oauth_html_template(buff, text);
}

void oauth_response_2fa_template(shmap_t *sess, shbuf_t *buff, char *client_id)
{
  char text[20480];
  char key_str[MAX_SHARE_HASH_LENGTH];
  char *token;


  token = http_token_encode(oauth_sess_token(sess));

  memset(text, 0, sizeof(text));
  snprintf(text, sizeof(text)-1, _oauth_response_2fa_html, 
    _neo_natura_logo, oauth_app_header_html(sess, client_id),
    _person_icon, client_id, token);

  free(token);

  oauth_html_template(buff, text);
}

char *http_entity_encode(char *text)
{
  char *html;
  int len;
  int i;
  int j;

  html = (char *)calloc(strlen(text) * 6, sizeof(char));

  j = 0;
  for (i = 0; i < strlen(text); i++) {
    if (strchr("\"&'<>@", text[i])) {
      len = sprintf(html + j, "&#x%x;", (int)text[i]);
      j += len;
    } else {
      html[j++] = text[i];
    }
  }

  return (html);
}

void oauth_register_2fa_template(shmap_t *sess, shbuf_t *buff, char *client_id)
{
  char text[20480];
  char key_str[256];
  char buf[256];
  char secret_url[256];
  char *ent_url;
  char uri[1024];
  char username[MAX_SHARE_NAME_LENGTH];
  char *secret;
  char *url;
  char *str;
  char *token;
  time_t now;

fprintf(stderr, "DEBUG: oauth_register_2fa_template()\n");

  str = shmap_get_str(sess, ashkey_str("username"));
  if (!str) {
fprintf(stderr, "DEBUG: oauth_register_2fa_template: NULL session\n");
    oauth_response_token_template(sess, buff, client_id);
    return; /* invalid */
  }
  memset(username, 0, sizeof(username));
  strncpy(username, str, sizeof(username) - 1);


  secret = oauth_sess_2fa_secret(sess);
  token = http_token_encode(oauth_sess_token(sess));

  sprintf(uri, "otpauth://totp/%s?secret=%s&issuer=sharenet", username, secret);
  url = http_token_encode(uri);
  sprintf(secret_url, "https://www.google.com/chart?chs=200x200&chld=M|0&cht=qr&chl=%s", url);
  free(url);

  ent_url = http_entity_encode(secret_url);
  memset(text, 0, sizeof(text));
  snprintf(text, sizeof(text)-1, _oauth_register_2fa_html, 
      _neo_natura_logo, _person_icon, ent_url, 
      username, secret, client_id, token);
  free(ent_url);

  free(token);

  oauth_html_template(buff, text);
}

char *oauth_app_header_html(shmap_t *sess, char *client_id)
{
  static char ret_buf[1024];
  char *app_logo = oauth_sess_client_logo(sess, client_id);
  char *app_title = oauth_sess_client_title(sess, client_id);

  memset(ret_buf, 0, sizeof(ret_buf));

#if 0
  if (!app_title || !*app_title)
    return (ret_buf); /* invalid */
#endif

  strcpy(ret_buf,
      "<div style=\"clear : both;\"></div>\r\n"
      "\r\n"
      "<div style=\"float : left; margin-left : 32px; padding : 16px 16px 16px 16px;\">\r\n");

  /* app logo */
  sprintf(ret_buf+strlen(ret_buf),
      "<img src=\"%s\" width=140 height=80 style=\"border : 0; outline : 0;\">\r\n",
      (app_logo && *app_logo) ? app_logo :
      "data:image/gif;base64,R0lGODlhAQABAAD/ACwAAAAAAQABAAACADs=");

  sprintf(ret_buf+strlen(ret_buf),
      "</div>\r\n"
      "<div style=\"float : left; margin-left : 32px; font-size : 28px; height : 20px; margin-top : 75px;\">\r\n"
      "<span>%s</span>\r\n"
      "</div>\r\n"
      "\r\n"
      "<div style=\"clear : both;\"></div>\r\n",
      app_title ? app_title : "");

  return (ret_buf);
}

void oauth_admin_user_template(shmap_t *sess, shbuf_t *buff, char *client_id, char *warning)
{
  char *app_title = oauth_sess_client_title(sess, client_id);
  char text[20480];
  char chk_2fa_on[32];
  char chk_2fa_off[32];
char username[256];
char fullname[256];
char address[256];
char zipcode[256];
char phone[256];
  char *str;

  str = shmap_get_str(sess, ashkey_str("2fa"));
  if (str && 0 == strcmp(str, "1"))
    strcpy(chk_2fa_on, " CHECKED");
  else
    strcpy(chk_2fa_off, " CHECKED");

  memset(username, 0, sizeof(username));
  str = shmap_get_str(sess, ashkey_str("username"));
  if (str)
    strncpy(username, str, sizeof(username)-1);

  memset(fullname, 0, sizeof(fullname));
  str = shmap_get_str(sess, ashkey_str("fullname"));
  if (str)
    strncpy(fullname, str, sizeof(fullname)-1);

  memset(address, 0, sizeof(address));
  str = shmap_get_str(sess, ashkey_str("address"));
  if (str)
    strncpy(address, str, sizeof(address)-1);

  memset(zipcode, 0, sizeof(zipcode));
  str = shmap_get_str(sess, ashkey_str("zipcode"));
  if (str)
    strncpy(zipcode, str, sizeof(zipcode)-1);

  memset(phone, 0, sizeof(phone));
  str = shmap_get_str(sess, ashkey_str("phone"));
  if (str)
    strncpy(phone, str, sizeof(phone)-1);

  memset(text, 0, sizeof(text));
  snprintf(text, sizeof(text)-1, _oauth_admin_user_html, 
    _neo_natura_logo, warning ? warning : "", username,
    fullname, address, zipcode, phone,
    client_id, chk_2fa_on, chk_2fa_off);

  oauth_html_template(buff, text);
}

