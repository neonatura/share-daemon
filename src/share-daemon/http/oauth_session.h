
#ifndef __OAUTH_SESSION_H__
#define __OAUTH_SESSION_H__

int oauth_sess_login_verify(shd_t *cli, shmap_t *sess, char *username, char *password);

shmap_t *oauth_sess_load(shd_t *cli, char *client_id);

int oauth_sess_login(shmap_t *sess);

int oauth_sess_access(shmap_t *sess, char *client_id, int scope);

void oauth_sess_access_grant(shmap_t *sess, char *client_id, int scope);

char *oauth_sess_token(shmap_t *sess);

char *oauth_sess_redirect_url(shmap_t *sess, char *client_id);

int oauth_sess_scope(shmap_t *sess, char *client_id);

int oauth_sess_scope_str(shmap_t *sess, char *client_id);

int oauth_sess_scope_set(shmap_t *sess, char *client_id, char *scope_str);

char *oauth_sess_client_title(shmap_t *sess, char *client_id);

char *oauth_sess_client_logo(shmap_t *sess, char *client_id);

char *oauth_sess_username(shmap_t *sess);

void oauth_sess_redirect_url_set(shmap_t *sess, char *client_id, char *redirect_url);

char *oauth_sess_2fa_secret(shmap_t *sess);

char *oauth_sess_client_id(shmap_t *sess);

char *http_token_decode(char *string);

shmap_t *oauth_sess_find(char *token);

shkey_t *oauth_sess_id(shd_t *cli);


#endif /* ndef __OAUTH_SESSION_H__ */

