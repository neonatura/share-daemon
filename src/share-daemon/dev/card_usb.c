
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura
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
 *
 *  @endcopyright
 */  

#include "sharedaemon.h"

#define SHARE_CARD_POLL_TIME 10



/**
 * Writes up to 337 bytes of raw interrupt data.
 * @param The buffer to write the incoming data stream.
 */
int card_usb_read_io(shdev_t *c_dev)
{
#ifdef HAVE_USB
  char rbuf[64];
  char buf[64];
  unsigned int r_len;
  int code;
  int err;

  r_len = 0;
  memset(rbuf, 0, sizeof(rbuf));
  err = libusb_interrupt_transfer(c_dev->usb, 
      (1 | LIBUSB_ENDPOINT_IN), rbuf, 8, &r_len, SHARE_CARD_POLL_TIME);
  if (err != LIBUSB_SUCCESS) {
    if (err == -4)
      return (SHERR_NOMEDIUM); /* unplugged */
    if (err == -7) 
      return (SHERR_AGAIN);
    fprintf(stderr, "DEBUG: libusb_interrupt_transfer: err %d (%s)\n", err, libusb_error_name(err));
    return (SHERR_IO);
  }
  if (r_len != 8) {
    fprintf(stderr, "DEBUG: r_len == 0, breaking..\n");
    return (SHERR_IO);
  }

  code = card_kmap_convert(rbuf);
  if (code == 0) {
    return (SHERR_AGAIN);
}

  buf[0] = code;
  shbuf_cat(c_dev->buff, buf, 1);

  if (code == '\n')
    fprintf(stderr, "INPUT: \"%s\"\n", shbuf_data(c_dev->buff));

#endif
  return (0);
}

int card_usb_fill_lic(shcard_t *card, char *data)
{
  return (SHERR_INVAL);
}

int card_usb_fill_fin_sc(shcard_t *card, int *sc)
{
  if (sc[0] == 1 || sc[0] == 2)
    card->card_flags |= SHCARD_INTERNATIONAL;
  else if (sc[0] == 5 || sc[0] == 6)
    card->card_flags |= SHCARD_NATIONAL;
  if (sc[0] == '2' || sc[0] == 6)
    card->card_flags |= SHCARD_CHIP; 

  if (sc[1] == 2)
    card->card_flags |= SHCARD_ONLINE;
  else if (sc[1] == 4)
    card->card_flags |= SHCARD_ONLINE_BILATERAL;

  if (sc[2] == 0 || sc[2] == 3 || sc[2] == 5)
    card->card_flags |= SHCARD_PIN;
  else if (sc[2] == 6 || sc[2] == 7)
    card->card_flags |= SHCARD_PIN_PREF;
  if (sc[2] == 2 || sc[2] == 5 || sc[2] == 7)
    card->card_flags |= SHCARD_SERVICE; /* !CASH */
  else if (sc[2] == 3)
    card->card_flags |= SHCARD_ATM;
  else if (sc[2] == 4)
    card->card_flags |= SHCARD_CASH;
}

void card_usb_fill_fin_type(shcard_t *card, char *id_str)
{
  char card_type[32];

  memset(card_type, 0, sizeof(card_type));
  if (0 == strncmp(id_str, "34", 2) ||
      0 == strncmp(id_str, "37", 2)) {
    strcpy(card_type, CARDTYPE_AMEX);
  } else if (0 == strncmp(id_str, "6011", 4) ||
      0 == strncmp(id_str, "622", 3) || 
      0 == strncmp(id_str, "64", 2) ||
      0 == strncmp(id_str, "65", 2)) {
    strcpy(card_type, CARDTYPE_DISCOVER);
  } else if (0 == strncmp(id_str, "50", 2) ||
      0 == strncmp(id_str, "56", 2) ||
      0 == strncmp(id_str, "57", 2) ||
      0 == strncmp(id_str, "58", 2) ||
      0 == strncmp(id_str, "59", 2) ||
      0 == strncmp(id_str, "6", 1)) {
    strcpy(card_type, CARDTYPE_MAESTRO);
  } else if (0 == strncmp(id_str, "51", 2) ||
      0 == strncmp(id_str, "52", 2) ||
      0 == strncmp(id_str, "53", 2) ||
      0 == strncmp(id_str, "54", 2) ||
      0 == strncmp(id_str, "55", 2)) {
    strcpy(card_type, CARDTYPE_MASTER);
  } else if (0 == strncmp(id_str, "4", 1)) {
    strcpy(card_type, CARDTYPE_VISA);
  }

  strncpy(card->card_type, card_type, sizeof(card->card_type)-1);
}

int card_usb_fill_fin(shcard_t *card, char *data)
{
  shkey_t *key;
  struct tm *cur;
  struct tm t;
  time_t now;
  char id_str[256]; /* max 19b */ 
  char id_name[256]; /* max 26b */
  char y_str[32];
  char m_str[32];
  uint64_t id_num;
  int sc[3]; /* service code */
  int idx;

  now = time(NULL);
  cur = gmtime(&now);

  /* parse id number */
  idx = stridx(data, '^');
  if (idx == -1) return (SHERR_INVAL);
  memset(id_str, 0, sizeof(id_str));
  strncpy(id_str, data, idx);
  id_num = atoll(id_str);
  data += (idx + 1);

  /* parse name info */
  idx = stridx(data, '^');
  if (idx == -1) return (SHERR_INVAL);
  memset(id_name, 0, sizeof(id_name));
  strncpy(id_name, data, idx);
  strtok(id_name, " ");

  /* parse expiration */
  y_str[0] = data[idx+1];
  y_str[1] = data[idx+2];
  y_str[2] = '\0';
  m_str[0] = data[idx+3];
  m_str[1] = data[idx+4];
  m_str[2] = '\0';
  memset(&t, 0, sizeof(t));
  t.tm_year = atoi(y_str) + 100;
  t.tm_mon = atoi(m_str) - 1;
  t.tm_mday = 1;
  t.tm_isdst = -1;
  if (t.tm_year < cur->tm_year ||
      (t.tm_year == cur->tm_year && t.tm_mon <= cur->tm_mon)) {
time_t tim = mktime(&t); fprintf(stderr, "DEBUG: CARD EXPIRED '%19.19s'\n", ctime(&tim)+4);
    return (SHERR_KEYEXPIRED);
  }

  /* service code flags */
  sc[0] = data[idx+5] - '0';
  sc[1] = data[idx+6] - '0';
  sc[2] = data[idx+7] - '0';
  card_usb_fill_fin_sc(card, (int *)sc);

  /* fill card credentials. */
  key = shkey_bin((char *)&id_num, sizeof(id_num));
  memcpy(&card->card_id, key, sizeof(shkey_t)); 
  shkey_free(&key);
  card->card_acc = shpam_uid(id_name);
  card->card_expire = shmktime(&t);
  card_usb_fill_fin_type(card, id_str);
//fprintf(stderr, "DEBUG: CARD ID '%s' KEY '%s' EXPIRE '%s' ACC(%llu) FLAGS(%d) TYPE(%s)\n", id_str, shkey_print(&card->card_id), shctime(card->card_expire), card->card_acc, card->card_flags, card->card_type);

  return (0);
}

int card_usb_fill(shcard_t *card, char *data)
{


  if (strlen(data) < 17)
    return (SHERR_INVAL);

  if (data[0] == '%') {
    if (isalpha(data[1]) && isalpha(data[2])) {
      return (card_usb_fill_lic(card, data + 1));
    } 
    if (data[1] == 'B') {
      return (card_usb_fill_fin(card, data + 2));
    }
  } 

  return (SHERR_INVAL);
}

int card_usb_read(shdev_t *c_dev)
{
  char *data;
  char buf[1024];
  int idx;
  int err;

  err = card_usb_read_io(c_dev);
  if (err) {
if (err == SHERR_NOMEDIUM) {
/* shut down device */
c_dev->err_state = err;
}
if (err != SHERR_AGAIN) fprintf(stderr, "DEBUG: card_usb_read_io: err %d\n", err);
    return (err);
}

  data = shbuf_data(c_dev->buff);
  if (!shbuf_size(c_dev->buff) ||
      -1 == (idx = stridx(data, '\n')) || idx > 337) {
    return (SHERR_AGAIN);
  }

  memset(buf, 0, sizeof(buf));
  strncpy(buf, data, idx);
  shbuf_trim(c_dev->buff, idx+1);

  err = card_usb_fill(&c_dev->data.card, buf);
  if (err)
    return (err);
  c_dev->index++;

  return (0);
}

#if 0
int main(void)
{
  shdev_t *c_dev;
  int vid = 0x0801;
  int pid = 0x0001;
  int err;

  libusb_init(NULL);

  c_dev = card_usb_open(vid, pid);
if (c_dev) {

  do {
    err = card_usb_read(c_dev);
  } while (err == SHERR_AGAIN);

  card_usb_close(c_dev);
}

  libusb_exit(NULL);

  return (0);
}
#endif

int shdev_card_init(shdev_t *dev)
{
  return (0);
}
int shdev_card_start(shdev_t *dev)
{
  return (0);
}
int shdev_card_poll(shdev_t *dev)
{
  int err;

  err = card_usb_read(dev);
  if (err)
    return (err);
  
  return (0);
}
int shdev_card_timer(shdev_t *dev)
{
}
int shdev_card_shutdown(shdev_t *dev)
{
}
