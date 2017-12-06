
/*
 *  Copyright 2013 Neo Natura 
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

#ifndef __SHAREDAEMON_STORE_H__
#define __SHAREDAEMON_STORE_H__

int pstore_save_tx(tx_t *tx);

int pstore_save(void *data, size_t data_len);

void *pstore_load(int op, char *hash);

void pstore_free(void *tx);

/**
 * Remove a transaction from persistent storage.
 */
int pstore_delete_tx(tx_t *tx);


#endif /* ndef __SHAREDAEMON_STORE_H__ */
