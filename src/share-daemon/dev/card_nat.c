
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


void card_nat_fill(shcard_t *card, shadow_t *shadow)
{
  shpeer_t *peer;

  card->card_expire = shadow->sh_expire;
  card->card_acc = shadow->sh_uid;
  strcpy(card->card_type, CARDTYPE_NEONATURA);

  peer = shpeer();
  memcpy(&card->card_issuer, peer, sizeof(shpeer_t));
  shpeer_free(&peer);

}


