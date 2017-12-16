
int oauth_admin_api_user(shd_t *cli, char *client_id, char *password, char *fullname, char *address, char *zipcode, char *phone, int b_2fa);

int oauth_admin_api_client(shd_t *cli, char *client_id, char *title, char *logo_url);

int oauth_admin_user(shd_t *cli, char *client_id, char *password, char *fullname, char *address, char *zipcode, char *phone, int b_2fa);
