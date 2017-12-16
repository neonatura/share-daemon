



int oauth_response_token(shd_t *cli, shbuf_t *buff, char *client_id, char *redirect_uri, char *scope);

int oauth_response_password(shd_t *cli, char *client_id, char *username, char *password, int enable_2fa);

int oauth_response_2fa(shd_t *cli, char *token, char *client_id, char *code, int enable_2fa);

int oauth_token_authorization_code(shd_t *cli, char *client_id, char *client_secret, char *auth_code, char *redirect_uri);
