
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


/**
 * Verifies whether a NAT-CSC code pertains to the card information provided.
 * @returns TRUE when csc code provided is valid and FALSE when invalid.
 */
int card_csc_ver(shcard_t *card, int csc)
{
  int ver_csc = card_csc_gen(card);

  if (ver_csc == csc)
    return (TRUE);

  return (FALSE);
}

/**
 * Generate a NAT-CSC code from a card's information. 
 * @param card The card's identifying information.
 * @returns A negative error code or a positive 3-digit CSC code.
 * @note This CSC code should not be retained (generate as needed). 
 */
int card_csc_gen(shcard_t *card)
{
  shcard_t enc_card;
  shkey_t *enc_key;
  uint64_t issuer;
  char buf[1024];
  size_t len;
  int csc;
int err;

  memset(buf, 0, sizeof(buf));
  memcpy(buf, card, BASE_CARD_SIZE);

  /* four-digit issuer (card number's "3" through "6" sequence) */
  issuer = shcrc(shpeer_kpriv(&card->card_issuer), sizeof(shkey_t)) % 10000;
  issuer = htonl(issuer); /* add some girth */
  enc_key = shkey_num(issuer);

  len = BASE_CARD_SIZE;
  err = ashencode(buf, &len, enc_key);
  shkey_free(&enc_key);
  if (err)
    return (err);

  /* generate secure "24bit" csc */
  len = BASE_CARD_SIZE;
  csc = (int)abs(shcrc(buf, len) % 1000);

  return (csc);
}
