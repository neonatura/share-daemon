
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

#ifndef __HTTP__OAUTH_H__
#define __HTTP__OAUTH_H__


/** Permission to access user's personal info. */
#define OAUTH_SCOPE_INFO (1 << 0)
/** Permission to access user's mobile or static location. */
#define OAUTH_SCOPE_LOCATION (1 << 1)
/** Permission to access user's home sharefs partition. */
#define OAUTH_SCOPE_MEDIA (1 << 2)
/** Permission to access external scanning peripheral. */
#define OAUTH_SCOPE_SCAN (1 << 3)
/** Permissions to initiate usde currency transactions */
#define OAUTH_SCOPE_WALLET (1 << 4)

#define MAX_OAUTH_SCOPE 5


#endif /* ndef __HTTP__OAUTH_H__ */


