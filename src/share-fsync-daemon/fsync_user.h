
/*
 *  Copyright 2014 Neo Natura 
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

#ifndef __FSYNC_USER_H__
#define __FSYNC_USER_H__


void fsync_user_init(void);

fuser_t *fsync_user_add(int uid, char *username, char *userpass, char *path);

int fsync_user_validate(fuser_t *u, char *pass);

void fsync_user_scan(void);

void fsync_user_free(void);



#endif /* ndef __FSYNC_USER_H__ */

