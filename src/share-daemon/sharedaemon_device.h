
/*
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
 */  

#ifndef __SHAREDAEMON_DEVICE_H__

#define SHDEV_MAGTEK_VID 0x0801
#define SHDEV_MAGTEK_PID 0x0001

#define SHDEV_ZTEX_VID 0x221A
#define SHDEV_ZTEX_PID 0x0100

#define MAX_DEVICE_DEFINITIONS 4

extern shdev_t *sharedaemon_device_list;
extern shdev_def_t device_def[MAX_DEVICE_DEFINITIONS];

int sharedaemon_device_control(shdev_t *dev);

#endif
