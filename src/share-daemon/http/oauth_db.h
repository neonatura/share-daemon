
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

#ifndef __HTTP__OAUTH_DB_H__
#define __HTTP__OAUTH_DB_H__



#define MAX_OAUTH_FLAGS 1

#define OAF_2FA (1 << 0)

#define OAUTH_USER_TABLE "consumer"
#define OAUTH_APP_TABLE "client"
#define OAUTH_SESSION_TABLE "session"


typedef struct oauth_user_t
{
  char *username;
  char *fullname;
  char *address;
  char *zipcode;
  char *phone;
  char *secret;
  int flags;
} oauth_user_t;

typedef struct oauth_app_t
{
  char *client_id;
  char *title;
  char *logo_url;
  int flags;  
} oauth_app_t;

static const char *_oauth_flag_label[MAX_OAUTH_FLAGS] = 
{
  "2FA"
}; 

/**
 * An instance of a particular consumer linked to a particular app.
 */
typedef struct oauth_sess_t
{
  char *client_id;
  char *username;
  char *perm; /* granted access */
  char *host;
  shgeo_t geo;
  shtime_t stamp;
} oauth_sess_t;




shdb_t *oauth_db_init(void);
void oauth_db_term(void);
int oauth_userdb_free(oauth_user_t **user_p);
int oauth_userdb_save(struct oauth_user_t *user);
oauth_user_t *oauth_userdb_init(char *username);
oauth_user_t *oauth_userdb_load(char *username);

oauth_app_t *oauth_appdb_load(char *client_id);

oauth_app_t *oauth_appdb_init(char *client_id);

int oauth_appdb_save(struct oauth_app_t *app);

int oauth_appdb_free(oauth_app_t **app_p);
 



#endif /* ndef _HTTP__OAUTH_DB_H__ */
