
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


int shdev_usb_init(shdev_t *c_dev)
{
#ifdef USE_USB
  struct libusb_device *dev = NULL;
  struct libusb_config_descriptor *conf_desc;
  libusb_device_handle *hndl = NULL;
  char ebuf[1024];
  char buf[4096];
  char rbuf[1024];
  uint16_t max_len;
  unsigned int r_len;
  unsigned int len;
  int nb_ifaces;
  int val;
  int code;
  int iface;
  int err;
  int i;

  hndl = libusb_open_device_with_vid_pid(NULL, c_dev->def->arg1, c_dev->def->arg2);
  if (!hndl)
    return (SHERR_IO);

#ifdef __linux__
  /* detach kernel from device. */
  libusb_detach_kernel_driver(hndl, 0);
#endif

  /* Claim the device if it lists one or more interfaces available. */
  dev = libusb_get_device(hndl);
  libusb_get_config_descriptor(dev, 0, &conf_desc);
  nb_ifaces = conf_desc->bNumInterfaces;
  if (nb_ifaces) {
    for (iface = 0; iface < nb_ifaces; iface++) {
      err = libusb_claim_interface(hndl, iface);
      if (err != LIBUSB_SUCCESS)
        continue; /* in use */

      sprintf(ebuf, "claimed usb v%d:p%d (iface #%d).", c_dev->def->arg1, c_dev->def->arg2, iface);
      shinfo(ebuf);
fprintf(stderr, "DEBUG: %s\n", ebuf);
      break;
    }
    if (iface == nb_ifaces) {
      /* no slots left */
      return (SHERR_AGAIN);
    }
  }

  c_dev->usb = hndl;
  c_dev->iface = iface;
#endif
  return (0);
}

void shdev_usb_shutdown(shdev_t *c_dev)
{
#ifdef USE_USB

  if (!c_dev->usb)
    return;

  libusb_release_interface(c_dev->usb, c_dev->iface);
  libusb_close(c_dev->usb);

  c_dev->usb = NULL;
  c_dev->iface = 0;

#endif
}

