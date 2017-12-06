
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

#define PASS_CODE_LENGTH 6
#define SECRET_LENGTH 10
int PIN_MODULO = pow(10, PASS_CODE_LENGTH);

static uint32_t _hashToInt(unsigned char *data, size_t of)
{
  uint32_t *input;

  input = (uint32_t *)(data + of);
  *input = ntohl(*input);

  return *(input);
}

int oauth_2fa_verify(char *secret, char *code)
{
  time_t t;
  int i;

  if (!secret)
    return (FALSE);

  t = time(NULL) / 30;
  for ( i = -1; i <= 1; i++) { /* drift compensation */
fprintf(stderr, "DEBUG: oauth_2fa_verify (secret '%s'): code '%s' vs user code '%s'\n", secret, oauth_2fa_code(secret, t + i), code);
    if (0 == strcasecmp(oauth_2fa_code(secret, t + i), code))
      return (TRUE);
  }

  return (FALSE);
}

char *oauth_2fa_code(char *secret_str, time_t t)
{
  static char ret_pin[32];
  unsigned char hash[512];
  unsigned char secret[256];
  uint64_t t_val;
  uint32_t hash32;
  size_t secret_len;
  size_t of;
  int ok;
  int i;

  memset(ret_pin, 0, sizeof(ret_pin));

  if (!secret_str || strlen(secret_str) != 16)
    return (ret_pin); /* invalid */

  for (i = 0; i < 16; i++) {
    secret_str[i] = toupper(secret_str[i]);
  } 

  memset(secret, 0, sizeof(secret));
  secret_len = sizeof(secret);
  ok = shbase32_decode(secret_str, strlen(secret_str), secret, &secret_len);
  if (!ok) /* decode failure */
    return (ret_pin);

  if (!t)
    t = time(NULL) / 30;
  t_val = (uint64_t)t;
  t_val = htonll(t_val);

#if 0
  memset(&sha1, 0, sizeof(sha1));
  shsha1_initHmac(&sha1, secret, secret_len);
  shsha1_write(&sha1, (char *)&t_val, sizeof(t_val));
  hash = shsha1_resultHmac(&sha1); /* 20 bytes */
#endif
  shhmac(SHALG_SHA1, secret, secret_len, (unsigned char *)&t_val, sizeof(t_val), hash);

  of = (int)hash[19] & 0xf;
  hash32 = (_hashToInt(hash, of) & 0x7FFFFFFF);
  hash32 = hash32 % PIN_MODULO;
  sprintf(ret_pin, "%-*.*u", PASS_CODE_LENGTH, PASS_CODE_LENGTH, hash32);

  return (ret_pin);
}

char *oauth_2fa_secret(shkey_t *key)
{
  static char ret_buf[64];
  unsigned char *key_data;
  size_t key_strlen;

  if (!key)
    key = ashkey_uniq();
  key_data = (unsigned char *)key->code;

  key_strlen = sizeof(ret_buf);
  memset(ret_buf, 0, sizeof(ret_buf));
  shbase32_encode(key_data, SECRET_LENGTH, ret_buf, key_strlen-1);

  return (ret_buf);
}


