
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



static shdb_t *_oauth_db;

shdb_t *oauth_db_init(void)
{
  int err;

  if (!_oauth_db) {
    err = shdb_init("oauth", &_oauth_db);
    if (err)
      return (NULL);

    if (0 == shdb_table_new(_oauth_db, OAUTH_USER_TABLE)) {
      shdb_col_new(_oauth_db, OAUTH_USER_TABLE, "username");
      shdb_col_new(_oauth_db, OAUTH_USER_TABLE, "fullname");
      shdb_col_new(_oauth_db, OAUTH_USER_TABLE, "address");
      shdb_col_new(_oauth_db, OAUTH_USER_TABLE, "zipcode");
      shdb_col_new(_oauth_db, OAUTH_USER_TABLE, "phone");
      shdb_col_new(_oauth_db, OAUTH_USER_TABLE, "secret");
      shdb_col_new(_oauth_db, OAUTH_USER_TABLE, "flags");
    }
    if (0 == shdb_table_new(_oauth_db, OAUTH_APP_TABLE)) {
      shdb_col_new(_oauth_db, OAUTH_APP_TABLE, "client_id");
      shdb_col_new(_oauth_db, OAUTH_APP_TABLE, "title");
      shdb_col_new(_oauth_db, OAUTH_APP_TABLE, "logo_url");
      shdb_col_new(_oauth_db, OAUTH_APP_TABLE, "flags");
    }
  }

  return (_oauth_db);
}

void oauth_db_term(void)
{
  if (_oauth_db) {
    shdb_close(_oauth_db);
    _oauth_db = NULL;
  }
}

oauth_user_t *oauth_userdb_init(char *username)
{
  oauth_user_t *user;

  user = (oauth_user_t *)calloc(1, sizeof(oauth_user_t));
  if (!user) {
    return (NULL);
  }

  user->username = strdup(username);

  return (user);
}

oauth_user_t *oauth_userdb_load(char *username)
{
  oauth_user_t *user;
  shdb_t *db;
  shdb_idx_t row_id;
  char *str;
  int err;
  int i;

  db = oauth_db_init();
  if (!db)
    return (NULL);

  err = shdb_row_find(db, OAUTH_USER_TABLE, &row_id, "username", username, 0);
  if (err) {
fprintf(stderr, "DEBUG: oauth_userdb_load: error on '%s' username lookup\n", username);
    return (NULL);
  }

  user = oauth_userdb_init(username);
  if (!user) {
fprintf(stderr, "DEBUG: oauth_userdb_load: oauth_user_db_init() failure\n");
    return (NULL);
}

  user->fullname = shdb_row_value(db, OAUTH_USER_TABLE, row_id, "fullname");
  user->address = shdb_row_value(db, OAUTH_USER_TABLE, row_id, "address");
  user->zipcode = shdb_row_value(db, OAUTH_USER_TABLE, row_id, "zipcode");
  user->phone = shdb_row_value(db, OAUTH_USER_TABLE, row_id, "phone");
  user->secret = shdb_row_value(db, OAUTH_USER_TABLE, row_id, "secret");
fprintf(stderr, "DEBUG: loaded user secret '%s'\n", user->secret);

  str = shdb_row_value(db, OAUTH_USER_TABLE, row_id, "flags");
  if (str) {
    for (i = 0; i < MAX_OAUTH_FLAGS; i++) {
      if (strstr(str, _oauth_flag_label[i]))
        user->flags |= (1 << i);
    }
    free(str);
  }

  return (user);
}

int oauth_userdb_free(oauth_user_t **user_p)
{
  oauth_user_t *user;

  if (!user_p)
    return;
  
  user = *user_p;
  *user_p = NULL;

  free(user->username);
  free(user->fullname);
  free(user->address);
  free(user->zipcode);
  free(user->phone);
  free(user->secret);
  free(user);

}

int oauth_userdb_save(struct oauth_user_t *user)
{
  shdb_t *db;
  shdb_idx_t row_id;
  char flag_str[256];
  int err;
  int i;

  if (!user)
    return (0); /* all done */

  if (!user->username)
    return (SHERR_INVAL); /* invalid */

  db = oauth_db_init();
  if (!db)
    return (SHERR_IO);

  err = shdb_row_find(db, OAUTH_USER_TABLE, &row_id,
      "username", user->username, 0);
  if (err) {
    err = shdb_row_new(db, OAUTH_USER_TABLE, &row_id);
    if (err) {
fprintf(stderr, "DEBUG: oauth_userdb_save: error creating new row in userdb\n");
      return (err);
    }

    err = shdb_row_set(db, OAUTH_USER_TABLE, row_id, "username", user->username);
    if (err) {
fprintf(stderr, "DEBUG: oauth_userdb_save: error setting 'username' field\n");
      return (err);
    }
fprintf(stderr, "DEBUG: oauth_userdb_save: new record for '%s' in userdb\n", user->username); 
  }

  if (user->fullname)
    shdb_row_set(db, OAUTH_USER_TABLE, row_id, "fullname", user->fullname);
  if (user->address)
    shdb_row_set(db, OAUTH_USER_TABLE, row_id, "address", user->address);
  if (user->zipcode)
    shdb_row_set(db, OAUTH_USER_TABLE, row_id, "zipcode", user->zipcode);
  if (user->phone)
    shdb_row_set(db, OAUTH_USER_TABLE, row_id, "phone", user->phone);
  if (user->secret)
    shdb_row_set(db, OAUTH_USER_TABLE, row_id, "secret", user->secret);

  memset(flag_str, 0, sizeof(flag_str));
  for (i = 0; i < MAX_OAUTH_FLAGS; i++) {
    if (user->flags & (1 << i)) {
      strcat(flag_str, _oauth_flag_label[i]);
      strcat(flag_str, " ");
    }
  }
  shdb_row_set(db, OAUTH_USER_TABLE, row_id, "flags", flag_str);
fprintf(stderr, "DEBUG: saved userdb record '%s'\n", user->username);

  /* ensure db is flushed */
  oauth_db_term();

  return (0);
}

oauth_app_t *oauth_appdb_load(char *client_id)
{
  oauth_app_t *app;
  shdb_t *db;
  shdb_idx_t row_id;
  char *str;
  int err;
  int i;

  db = oauth_db_init();
  if (!db)
    return (NULL);

  err = shdb_row_find(db, OAUTH_APP_TABLE, &row_id, "client_id", client_id, 0);
  if (err) {
    return (NULL);
  }

  app = (oauth_app_t *)calloc(1, sizeof(oauth_app_t));
  if (!app) {
    return (NULL);
  }

  app->client_id = strdup(client_id);
  app->title = shdb_row_value(db, OAUTH_APP_TABLE, row_id, "title");
  app->logo_url = shdb_row_value(db, OAUTH_APP_TABLE, row_id, "logo_url");

  str = shdb_row_value(db, OAUTH_APP_TABLE, row_id, "flags");
  if (str) {
    for (i = 0; i < MAX_OAUTH_FLAGS; i++) {
      if (strstr(str, _oauth_flag_label[i]))
        app->flags |= (1 << i);
    }
    free(str);
  }

  return (0);
}

oauth_app_t *oauth_appdb_init(char *client_id)
{
  oauth_app_t *app;

  app = (oauth_app_t *)calloc(1, sizeof(oauth_app_t));
  if (!app) {
    return (NULL);
  }

  app->client_id = strdup(client_id);
  app->title = strdup("");
  app->logo_url = strdup("");

  return (app);
}

int oauth_appdb_free(oauth_app_t **app_p)
{
  oauth_app_t *app;

  if (!app_p)
    return;
  
  app = *app_p;
  *app_p = NULL;

  free(app->client_id);
  free(app->title);
  free(app->logo_url);
  free(app);

}

int oauth_appdb_save(struct oauth_app_t *app)
{
  shdb_t *db;
  shdb_idx_t row_id;
  char flag_str[256];
  int err;
  int i;

  if (!app)
    return (0); /* all done */

  if (!app->client_id)
    return (SHERR_INVAL); /* invalid */

  db = oauth_db_init();
  if (!db)
    return (SHERR_IO);

  err = shdb_row_find(db, OAUTH_APP_TABLE, &row_id,
      "client_id", app->client_id, 0);
  if (err) {
    err = shdb_row_new(db, OAUTH_APP_TABLE, &row_id);
    if (err) {
      return (err);
    }

    err = shdb_row_set(db, OAUTH_APP_TABLE, row_id, 
        "client_id", app->client_id);
    if (err) {
      return (err);
    }
  }

  app = (oauth_app_t *)calloc(1, sizeof(oauth_app_t));
  if (!app) {
    return (SHERR_NOMEM);
  }

  if (app->title)
    shdb_row_set(db, OAUTH_APP_TABLE, row_id, "title", app->title);
  if (app->logo_url)
    shdb_row_set(db, OAUTH_APP_TABLE, row_id, "logo_url", app->logo_url);

  memset(flag_str, 0, sizeof(flag_str));
  for (i = 0; i < MAX_OAUTH_FLAGS; i++) {
    if (app->flags & (1 << i)) {
      strcat(flag_str, _oauth_flag_label[i]);
      strcat(flag_str, " ");
    }
  }
  shdb_row_set(db, OAUTH_APP_TABLE, row_id, "flags", flag_str);

  /* ensure db is flushed */
  oauth_db_term();

  return (0);
}
