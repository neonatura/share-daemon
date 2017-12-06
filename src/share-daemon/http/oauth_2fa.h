
char *oauth_2fa_secret(shkey_t *key);
int oauth_2fa_verify(char *secret, char *code);
char *oauth_2fa_code(char *secret_str, time_t t);

