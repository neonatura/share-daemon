
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

#ifndef __DEV__CARD_USB_H__
#define __DEV__CARD_USB_H__


/**
 * @addtogroup sharedaemon_devicecard
 * @{
 */

/**
 * Initialize a card-swiper USB device.
 */
int shdev_card_init(shdev_t *dev);

/**
 * Poll from a card-swiper USB device.
 */
int shdev_card_poll(shdev_t *dev);

/**
 * Terminate a card-swiper USB device.
 */
int shdev_card_shutdown(shdev_t *dev);

/**
 * @}
 */


#endif /* ndef __DEV__CARD_USB_H__ */
