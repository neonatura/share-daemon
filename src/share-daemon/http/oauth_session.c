
#include "sharedaemon.h"

shmap_t *_session_table;

char *oauth_sess_hostname(shd_t *cli)
{
  static char host[MAXHOSTNAMELEN+1];
  struct sockaddr *addr;

  /* originating host */
  memset(host, 0, sizeof(host));
  addr = shaddr(cli->cli.net.fd);
  if (addr) {
    memset(host, 0, sizeof(host));
    getnameinfo(addr, sizeof(struct sockaddr), 
        host, sizeof(host)-1, NULL, 0, 0);
  }

  return (host);
}

shkey_t *oauth_sess_id(shd_t *cli)
{
  shmap_t *fields = cli->cli.net.fields;
  char host[MAXHOSTNAMELEN+1];
  char key_buf[1024];
  char *str;

  if (!fields)
    return (NULL); /* INVAL */

  memset(key_buf, 0, sizeof(key_buf));

  str = shmap_get_str(fields, ashkey_str("Accept-Language"));
  if (str)
    strncat(key_buf, str, sizeof(key_buf) - strlen(key_buf) - 1);

  str = shmap_get_str(fields, ashkey_str("Accept-Encoding"));
  if (str)
    strncat(key_buf, str, sizeof(key_buf) - strlen(key_buf) - 1);

  str = shmap_get_str(fields, ashkey_str("Accept"));
  if (str)
    strncat(key_buf, str, sizeof(key_buf) - strlen(key_buf) - 1);

  str = shmap_get_str(fields, ashkey_str("User-Agent"));
  if (str)
    strncat(key_buf, str, sizeof(key_buf) - strlen(key_buf) - 1);

  str = shmap_get_str(fields, ashkey_str("Host"));
  if (str)
    strncat(key_buf, str, sizeof(key_buf) - strlen(key_buf) - 1);

  /* originating host */
  strncat(key_buf, oauth_sess_hostname(cli),
      sizeof(key_buf) - strlen(key_buf) - 1);

  return (shkey_str(key_buf));
}

char *oauth_sess_username(shmap_t *sess)
{
  static char ret_str[MAX_SHARE_NAME_LENGTH];
  char *str;

  memset(ret_str, 0, sizeof(ret_str));
  str = shmap_get_str(sess, ashkey_str("username"));
  if (str)
    strncpy(ret_str, str, sizeof(ret_str) - 1);

  return (ret_str);
}

static shmap_t *oauth_sess_init(shd_t *cli, char *client_id)
{
  oauth_app_t *app;
  shmap_t *map;
  shkey_t *key;
  char key_str[1024];
  char hostname[MAXHOSTNAMELEN+1];

  if (!_session_table)
    _session_table = shmap_init();

  /* unique per user account */
  map = shmap_init();
  key = oauth_sess_id(cli);
  shmap_set_ptr(_session_table, key, map);
  shkey_free(&key);

  memset(key_str, '\000', sizeof(key_str));

  /* originating hostname */
  shmap_set_astr(map, ashkey_str("hostname"), oauth_sess_hostname(cli));

  if (client_id) {
    app = oauth_appdb_load(client_id);
    if (app) {
      if (app->title) {
        snprintf(key_str, sizeof(key_str)-1, "client/%s/app_title", client_id);
        shmap_set_astr(map, ashkey_str(key_str), app->title); 
      }
      if (app->logo_url) {
        snprintf(key_str, sizeof(key_str)-1, "client/%s/app_logo", client_id);
        shmap_set_astr(map, ashkey_str(key_str), app->logo_url);
      }
      oauth_appdb_free(&app);
    }
  }


  return (map);
}


shmap_t *oauth_sess_load(shd_t *cli, char *client_id)
{
  shmap_t *map;
  shkey_t *key;

  if (!cli)
    return (NULL);

  if (!_session_table)
    _session_table = shmap_init();

  /* unique per user session id and client app id */
  key = oauth_sess_id(cli);
  map = (shmap_t *)shmap_get_ptr(_session_table, key);
  shkey_free(&key);

  if (!map)
    map = oauth_sess_init(cli, client_id);

  return (map);
}

shmap_t *oauth_sess_find(char *token)
{
  shmap_t *map;
  shkey_t *key;
  shd_t *cli;


  if (!_session_table) {
    return (NULL);
}

  for (cli = sharedaemon_client_list; cli; cli = cli->next) {
    if (cli->cli.net.fd == 0)
      continue;
    if (!(cli->flags & SHD_CLIENT_HTTP))
      continue;

    key = oauth_sess_id(cli);
    map = (shmap_t *)shmap_get_ptr(_session_table, key);
    shkey_free(&key);
    if (!map)
      continue;

    if (0 == strcmp(oauth_sess_token(map), token))
      return (map);
  }

fprintf(stderr, "DEBUG: oauth_sess_find: token '%s' not found\n", token);
  return (NULL);
}

/** A boolean determination of whether a user is logged in. */
int oauth_sess_login(shmap_t *sess)
{
  char *token;
  char *login;
  int err;

  token = shmap_get_str(sess, ashkey_str("token"));

  login = shmap_get_str(sess, ashkey_str("login"));
  if (login && token && 0 == strcmp(login, token)) {
    return (TRUE);
  }

  return (FALSE);
}


/** A boolean determination of whether a particular access is allowed. */
int oauth_sess_access(shmap_t *sess, char *client_id, int scope)
{
  char key_str[256];
  char *token;
  char *login;
  int err;

  sprintf(key_str, "client/%s/access/%s", client_id, oauth_scope_label(scope));
fprintf(stderr, "DEBUG: oauth_sess_access: '%s' = '%s'\n", key_str, shmap_get_str(sess, ashkey_str(key_str)));
  if (shmap_get_str(sess, ashkey_str(key_str)))
    return (TRUE);

  return (FALSE);
}

void oauth_sess_access_grant(shmap_t *sess, char *client_id, int scope)
{
  char key_str[256];

  sprintf(key_str, "client/%s/access/%s", client_id, oauth_scope_label(scope));
fprintf(stderr, "DEBUG: oauth_sess_access_grant: granting '%s'\n", key_str);
  shmap_set_astr(sess, ashkey_str(key_str), "on");

#if 0
  user = oauth_sessdb_load(oauth_sess_username(sess));
  if (user) {
    oauth_sessdb_free(&user);
  }
#endif
}


char *oauth_sess_token(shmap_t *sess)
{
  static char ret_buf[MAX_SHARE_HASH_LENGTH];
  char *token;
  char *login;
  int err;

  token = shmap_get_str(sess, ashkey_str("token"));
  if (!token)
    return (NULL);

  memset(ret_buf, 0, sizeof(ret_buf));
  strncpy(ret_buf, token, sizeof(ret_buf)-1);

  return (ret_buf);
}


/**
 * Performs a login against the given username and password.
 * @returns 0 upon success or a sharelib error code
 */
int oauth_sess_login_verify(shd_t *cli, shmap_t *sess, char *username, char *password)
{
  oauth_user_t *user;
  shkey_t *sess_key;
  char token[1024];
  int err;

  shmap_unset(sess, ashkey_str("login"));

  sess_key = NULL;
  err = shuser_login(username, password, &sess_key);
//  err = shapp_account_login(username, password, &sess_key);
fprintf(stderr, "DEBUG: shapp_account_login: %d = shapp_account_login: user '%s' pass '%s'\n", err, username, password);
  if (err)
    return (err);

  shmap_set_astr(sess, ashkey_str("username"), username);

  memset(token, 0, sizeof(token));
  strncpy(token, shkey_print(sess_key), sizeof(token) - 1);
  shmap_set_astr(sess, ashkey_str("token"), token);
  shmap_set_astr(sess, ashkey_str("login"), token);

  user = oauth_userdb_load(username);
  if (!user) { 
    /* initialize user */
    user = oauth_userdb_init(username);
    /* initialize 2fa secret */
    user->secret = strdup(oauth_2fa_secret(ashkey_uniq())); 
fprintf(stderr, "DEBUG: generated new secret '%s'\n", user->secret);
    /* retain record */
    oauth_userdb_save(user);
  }

  /* 2fa */
  if (user->flags & OAF_2FA) {
    shmap_set_astr(sess, ashkey_str("2fa"), "on");
fprintf(stderr, "DEBUG: user has 2fa enabled.\n");
  } else { 
fprintf(stderr, "DEBUG: user has 2fa disabled.\n");
}
  shmap_set_astr(sess, ashkey_str("secret"), user->secret);
fprintf(stderr, "DEBUG: session secret is '%s'\n", user->secret);

  oauth_userdb_free(&user);

  return (0);
}

#ifndef ISXDIGIT 
#define ISXDIGIT isxdigit
#endif
/**
 * Parse RFC3986 encoded 'string' back to  unescaped version.
 *
 * @param string The data to be unescaped
 * @param olen unless NULL the length of the returned string is stored there.
 * @return decoded string or NULL
 * The caller must free the returned string.
 */
char *http_token_decode(char *string)
{
  size_t alloc, strindex=0;
  char *ns = NULL;
  unsigned char in;
  long hex;

  if (!string) return NULL;
  alloc = strlen(string)+1;
  ns = (char*) calloc(alloc, sizeof(char));
  if (!ns)
    return (NULL);

  while(--alloc > 0) {
    in = *string;
    if(('%' == in) && ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
      char hexstr[3]; // '%XX'
      hexstr[0] = string[1];
      hexstr[1] = string[2];
      hexstr[2] = 0;
      hex = strtol(hexstr, NULL, 16);
      in = (unsigned char)hex; /* hex is always < 256 */
      string+=2;
      alloc-=2;
    }
    ns[strindex++] = in;
    string++;
  }
  ns[strindex]=0;

  return (ns);
}

char *http_token_encode(char *string)
{
  size_t alloc, newlen;
  char *ns = NULL, *testing_ptr = NULL;
  unsigned char in;
  size_t strindex=0;
  size_t length;

  if (!string)
    return (strdup(""));

  alloc = strlen(string)+1;
  newlen = alloc;

  ns = (char*)calloc(alloc, sizeof(char));
  if (!ns)
    return (NULL);

  length = alloc-1;
  while(length--) {
    in = *string;

    switch(in){
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case 'a': case 'b': case 'c': case 'd': case 'e':
    case 'f': case 'g': case 'h': case 'i': case 'j':
    case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E':
    case 'F': case 'G': case 'H': case 'I': case 'J':
    case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case '_': case '~': case '.': case '-':
      ns[strindex++]=in;
      break;
    default:
      newlen += 2; /* this'll become a %XX */
      if(newlen > alloc) {
        alloc *= 2;
        testing_ptr = (char *)realloc(ns, alloc);
        ns = testing_ptr;
      }
      snprintf(&ns[strindex], 4, "%%%02X", in);
      strindex+=3;
      break;
    }
    string++;
  }
  ns[strindex]=0;

  return (ns);
}


char *oauth_sess_redirect_url(shmap_t *sess, char *client_id)
{
  static char uri[1024];
  char key_str[MAX_SHARE_HASH_LENGTH];
  char *str;

  memset(uri, 0, sizeof(uri));
  sprintf(key_str, "client/%s/uri", client_id);
  str = shmap_get_str(sess, ashkey_str(key_str));
  if (str)
    strncpy(uri, str, sizeof(uri) - 1);

  return (uri);
}

void oauth_sess_redirect_url_set(shmap_t *sess, char *client_id, char *redirect_url)
{
  static char uri[1024];
  char key_str[MAX_SHARE_HASH_LENGTH];
  char *str;

  sprintf(key_str, "client/%s/uri", client_id);
  shmap_set_astr(sess, ashkey_str(key_str), redirect_url);
}


int oauth_sess_scope(shmap_t *sess, char *client_id)
{
  char scope_str[MAX_SHARE_HASH_LENGTH + 16];
  char key_str[MAX_SHARE_HASH_LENGTH];
  char *str;

  memset(scope_str, 0, sizeof(scope_str));
  sprintf(key_str, "client/%s/scope", client_id);
  str = shmap_get_str(sess, ashkey_str(key_str));
  if (str)
    strncpy(scope_str, str, sizeof(scope_str) - 1); 

  return (oauth_scope_bits(scope_str));
}

int oauth_sess_scope_str(shmap_t *sess, char *client_id)
{
  static char ret_str[1024];
  int scope;
  int i;

  memset(ret_str, 0, sizeof(ret_str));
  scope = oauth_sess_scope(sess, client_id);
  for (i = 0; i < MAX_OAUTH_SCOPE; i++) {
    if (scope & (1 << i)) {
      strcat(ret_str, oauth_scope_label(1 << i));
      strcat(ret_str, " ");
    }
  }
  if (*ret_str && ret_str[strlen(ret_str)-1] == ' ')
    ret_str[strlen(ret_str)-1] = '\000';

  return (ret_str);
}

int oauth_sess_scope_set(shmap_t *sess, char *client_id, char *scope_str)
{
  char key_str[MAX_SHARE_HASH_LENGTH];

  sprintf(key_str, "client/%s/scope", client_id);
  if (scope_str)
    shmap_set_astr(sess, ashkey_str(key_str), scope_str);
  else
    shmap_unset(sess, ashkey_str(key_str));

}

char *oauth_sess_client_title(shmap_t *sess, char *client_id)
{
  static char app_title[MAX_SHARE_HASH_LENGTH];
  char key_str[MAX_SHARE_HASH_LENGTH];
  char *str;

  memset(app_title, '\000', sizeof(app_title));
  sprintf(key_str, "client/%s/app_title", client_id);
  str = shmap_get_str(sess, ashkey_str(key_str));
  if (str)
    strncpy(app_title, str, sizeof(app_title) - 1);

  return (app_title);
}

char *oauth_sess_client_logo(shmap_t *sess, char *client_id)
{
  static char app_logo[MAX_SHARE_HASH_LENGTH];
  char key_str[MAX_SHARE_HASH_LENGTH];
  char *str;
  time_t now;


  memset(app_logo, '\000', sizeof(app_logo));
  sprintf(key_str, "client/%s/app_logo", client_id);
  str = shmap_get_str(sess, ashkey_str(key_str));
  if (str)
    strncpy(app_logo, str, sizeof(app_logo) - 1);
fprintf(stderr, "DEBUG: oauth_response_access_template: client '%s': %s\n", client_id, app_logo);

  return (app_logo);
}


void oauth_sess_client_set(shmap_t *sess, char *client_id, char *title, char *logo_url)
{
  oauth_app_t *app;
  char key_str[256];

  app = oauth_appdb_load(client_id);
  if (!app) {
/* todo: prevent db buildup */
    app = oauth_appdb_init(client_id);
  }

  if (app->title)
    free(app->title);
  app->title = strdup(title ? title : "");

  if (app->logo_url)
    free(app->logo_url);
  app->logo_url = strdup(logo_url ? logo_url : "");

  oauth_appdb_save(app);

  snprintf(key_str, sizeof(key_str)-1, "client/%s/app_title", client_id);
  shmap_set_astr(sess, ashkey_str(key_str), app->title); 
  snprintf(key_str, sizeof(key_str)-1, "client/%s/app_logo", client_id);
  shmap_set_astr(sess, ashkey_str(key_str), app->logo_url);

fprintf(stderr, "DEBUG: client/%s/title=%s\n", client_id, app->title);
fprintf(stderr, "DEBUG: client/%s/app_logo=%s\n", client_id, app->logo_url);

  oauth_appdb_free(&app);
}

/**
 * The client id associated with a particular user's (consumer) account.
 */
char *oauth_sess_client_id(shmap_t *sess)
{
  static char ret_str[MAX_SHARE_HASH_LENGTH];
  shkey_t *ckey;
  shkey_t *skey;
  shkey_t *key;

  ckey = shkey_str(oauth_sess_username(sess));
  skey = shkey_str(oauth_sess_2fa_secret(sess));
  key = shkey_xor(ckey, skey);
  shkey_free(&ckey);
  shkey_free(&skey);

  memset(ret_str, '\000', sizeof(ret_str));
  strncpy(ret_str, shkey_print(key), sizeof(ret_str) - 1); 
  shkey_free(&key);
fprintf(stderr, "DEBUG: oauth_sess_client_id: '%s' + '%s' = '%s'\n", oauth_sess_username(sess), oauth_sess_2fa_secret(sess), ret_str);

  return (ret_str);
}

char *oauth_sess_2fa_secret(shmap_t *sess)
{
  static char ret_str[MAX_SHARE_HASH_LENGTH];
  char *str;

  memset(ret_str, 0, sizeof(ret_str));
  str = shmap_get_str(sess, ashkey_str("secret"));
  if (str)
    strncpy(ret_str, str, sizeof(ret_str)-1);

  return (ret_str);
}


