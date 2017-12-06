
/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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

static void get_tx_inode(tx_file_t *tx, shfs_t **fs_p, shfs_ino_t **ino_p)
{
  shfs_t *fs;
  shfs_ino_t *inode;

*fs_p = NULL;
*ino_p = NULL;

  if (!tx)
    return;

  fs = shfs_init(&tx->ino_peer);
  inode = shfs_file_find(fs, tx->ino_path);

  *fs_p = fs;
  *ino_p = inode;
}

static void update_tx_inode(tx_file_t *tx, SHFL *inode)
{
  tx->ino_mtime = shfs_mtime(inode);
  tx->ino_crc = shfs_crc(inode);
  tx->ino_size = shfs_size(inode);
}

static void set_tx_inode(tx_file_t *tx, SHFL *inode)
{
  shpeer_t *fs_peer = shfs_inode_peer(inode);
  shkey_t *ino_name = shfs_token(inode);
  char *ino_path = shfs_inode_path(inode);

  tx->ino_ctime = shfs_ctime(inode);
  strncpy(tx->ino_path, ino_path, SHFS_PATH_MAX - 1);
  memcpy(&tx->ino_peer, fs_peer, sizeof(shpeer_t));
  memcpy(&tx->ino_name, ino_name, sizeof(shkey_t));

  update_tx_inode(tx, inode);
}

static tx_file_t *prep_tx_file(tx_file_t *tx_file, int op_type, shfs_ino_t *inode, size_t data_of, size_t data_len)
{
  tx_fileseg_t seg;
  tx_file_t *file;
  shbuf_t *f_buff;
  shbuf_t *buff;
  size_t of;
  int err;

  buff = shbuf_init();
  shbuf_cat(buff, tx_file, sizeof(tx_file_t));

  file = (tx_file_t *)shbuf_data(buff);
  set_tx_inode(file, inode);
  file->ino_op = op_type;

  if (op_type == TXFILE_READ) {
  } else if (op_type == TXFILE_WRITE) {
    f_buff = shbuf_init();
    shfs_read_of(inode, f_buff, data_of, data_len);
    file->seg_len = MIN(data_len, shbuf_size(f_buff));
    file->seg_crc = shcrc(shbuf_data(f_buff), file->seg_len);
    shbuf_cat(f_buff, buff, file->seg_len);
    shbuf_free(&f_buff);
  }

  return ((tx_file_t *)shbuf_unmap(buff));
}



#if 0
int local_broadcast_file(tx_file_t *file)
{
fprintf(stderr, "DEBUG: local_broadcast_file[%s]: header<%d bytes) + data<%d bytes>\n", file->ino_path, sizeof(tx_file_t), file->ino_size);
  sched_tx(file, sizeof(tx_file_t) + file->ino_size);
  return (0);
}

/**
 * Conduct a file operation against connected peers.
 */
int broadcast_file_op(tx_file_t *file, int op, unsigned char *payload, size_t payload_len)
{
  tx_file_t *s_file;
  size_t len;
  int err;

  len = sizeof(tx_file_t) + payload_len;
  s_file = (tx_file_t *)calloc(1, len);
  if (!s_file)
    return (SHERR_NOMEM);

  memcpy(s_file, file, sizeof(tx_file_t));
  if (payload && payload_len)
    memcpy(((char *)s_file) + sizeof(tx_file_t), payload, payload_len); 

  s_file->ino_op = op;

  err = local_broadcast_file(s_file);
  if (err)
    return (err);

  free(s_file);

  return (0);
}
#endif



#if 0
/**
 * A file notification received from a client on the local machine.
 *
 * Inform remote hosts of the current file checksum.
 * Listening peers will return a TXFILE_READ, TXFILE_WRITE, or TXFILE_SYNC.
 * A TXFILE_READ indicates new data needs to be sent.
 * A TXFILE_WRITE indicates that local data should be over-written.
 * A TXFILE_SYNC indicates inode synchronization or willing non-compliance.
 * @note The blk argument will never contain data in this context.
 */
int local_file_notification(shpeer_t *peer, char *path)
{
  shfs_t *fs;
  shfs_ino_t *inode;
  tx_file_t *file;
  tx_file_t *send_tx;
  shbuf_t *buff;
  char sig_hash[MAX_SHARE_HASH_LENGTH];
  int err;


  fs = shfs_init(peer);
  inode = shfs_file_find(fs, path);

  strcpy(sig_hash, shkey_print(shfs_token(inode)));
  file = (tx_file_t *)pstore_load(TX_FILE, sig_hash);
  if (!file) {
    file = (tx_file_t *)calloc(1, sizeof(tx_file_t));
    if (!file)
      return (SHERR_NOMEM);

    set_tx_inode(file, inode);
    err = tx_init(NULL, (tx_t *)file, TX_FILE);
    if (err)
      return (err);

    pstore_save(file, sizeof(tx_file_t));
  } else {
    /* update inode state */
    update_tx_inode(file, inode);
  }

  send_tx = prep_tx_file(file, TXFILE_CHECKSUM, inode, 0, 0);
  if (send_tx) {
    /* broadcast checksum of synchronized file. */
    tx_send(NULL, (tx_t *)send_tx);
    free(send_tx);
  }

  pstore_free(file);

  return (0);
}
#endif

#if 0
#define SEG_CHECKSUM_SIZE 65536
int local_request_segments(shpeer_t *origin, tx_file_t *tx, SHFL *file)
{
  shbuf_t *buff;
  char data[SEG_CHECKSUM_SIZE];
  shsize_t max;
  shsize_t len;
  shsize_t of;
  int err;

  /* adjust size. */
  err = shfs_truncate(file, tx->ino.size);
  if (err)
    return (err);

  if (tx->ino.size > shfs_size(inode)) {
    /* request missing data suffix */
    tx->ino_op = TXFILE_READ;
    tx->ino_of = shfs_size(inode);
    tx->ino_crc = 0L;
    tx->ino_size = (tx->ino.size - shfs_size(inode));
    sched_tx_sink(shpeer_kpriv(origin), inode, sizeof(tx_file_t));
fprintf(stderr, "DEBUG: local_request_segements[suffix]: sched_tx_sink()\n"); 
  }


  of = 0;
  buff = shbuf_init();
  max = MIN(shfs_size(inode), tx->ino.size);
  for (of = 0; of < max; of += SEG_CHECKSUM_SIZE) {
    shbuf_clear(buff);
    len = MIN(SEG_CHECKSUM_SIZE, max - of);
    err = shfs_read_of(inode, buff, of, len);
    if (err)
      return (err); /* corruption ensues.. */

    /* request data segment. */
    tx->ino_op = TXFILE_READ;
    tx->ino_of = of;
    tx->ino_crc = shcrc(shbuf_data(buff), len);
    tx->ino_size = len;
    sched_tx_sink(shpeer_kpriv(origin), inode, sizeof(tx_file_t));
fprintf(stderr, "DEBUG: local_request_segements[prefix]: sched_tx_sink()\n"); 
  }
  shbuf_free(&buff);

  return (0);
}
#endif


int txfile_send_sync(shpeer_t *origin, tx_file_t *tx, shfs_ino_t *inode)
{
  tx_file_t s_tx;
  int err;

fprintf(stderr, "DEBUG: txfile_send_sync()\n");

  /* generate new sync transaction */
  memset(&s_tx, 0, sizeof(s_tx));
  s_tx.ino_op = TXFILE_SYNC;
  err = inittx_file(&s_tx, inode);
  if (err)
    return (err);

  /* notify originating host */
  err = tx_send(origin, (tx_t *)&s_tx); 
  if (err)
    return (err);

  return (0);
}

int txfile_send_read(shpeer_t *origin, 
    tx_file_t *tx, shfs_ino_t *inode, size_t of, size_t len)
{
  tx_file_t s_tx;
  shbuf_t *buff;
  uint64_t crc;
  int err;

  if (of > shfs_size(inode))
    return (SHERR_INVAL);

#if 0
  len = MIN(shfs_size(inode) - of, len);
  if (len == 0)
    return (0);
#endif

  crc = 0;
  buff = shbuf_init();
  if (of != shfs_size(inode)) {
    len = MAX(0, MIN(shfs_size(inode) - of, len));
    if (len != 0) {
      err = shfs_read_of(inode, buff, of, len);
      crc = shcrc(shbuf_data(buff), shbuf_size(buff));
      shbuf_free(&buff);
      if (err)
        return (err);
    }
  }

  /* generate new seg-crc transaction */
  memset(&s_tx, 0, sizeof(s_tx));
  s_tx.ino_op = TXFILE_READ;
  s_tx.seg_crc = crc;
  s_tx.seg_of = of;
  s_tx.seg_len = len;
  err = inittx_file(&s_tx, inode);
  if (err) {
fprintf(stderr, "DEBUG: txfile_send_read: %d = inittx_file()\n", err); 
    return (err);
}

  /* notify originating host */
  err = tx_send(origin, (tx_t *)&s_tx); 
  if (err)
    return (err);

  return (0);
}

/**
 * Process a TXFILE_READ request from a remote peer.
 */
int txfile_send_write(shpeer_t *origin, tx_file_t *tx)
{
  static tx_file_t blank_tx;
  shstat st;
  SHFL *inode;
  shfs_t *fs;
  tx_file_t *s_tx; 
  tx_bond_t *bond;
  shbuf_t *buff;
  size_t data_of;
  size_t data_len;
  double fee;
  int err;


  get_tx_inode(tx, &fs, &inode);
fprintf(stderr, "DEBUG: txfile_send_write: inode-size(%d) seg-len(%d) seg-of(%d)\n", shfs_size(inode), tx->seg_len, tx->seg_of); 
  err = shfs_fstat(inode, &st);
  if (err) {
fprintf(stderr, "DEBUG: txfile_send_write: %d = shfs_fstat()\n", err);
    return (err);
}

  fee = CALC_TXFILE_FEE(shfs_size(inode), shfs_ctime(inode));
  if (!NO_TXFILE_FEE(fee)) {
#if 0
/* DEBUG: */
    /* verify xfer is appropriated with bond. */
    bond = load_bond_peer(origin, NULL, shfs_peer(inode));
    if (!bond) {
      /* no bond has been established for xfer. */
      shfs_free(&fs);
      return (SHERR_CANCELED);
    }
    if (fee > BOND_CREDIT_VALUE(bond->bond_credit)) {
      /* bond is not appropriated enough for xfer. */
      free_bond(&bond);
      shfs_free(&fs);
      return (SHERR_ACCESS);
    }
/* DEBUG: */
#endif
  }

  data_of = tx->seg_of;
  if (data_of > shfs_size(inode))
    return (SHERR_INVAL);

  //data_len = MIN(shfs_size(inode) - data_of, data_len);
  data_len = MAX(0, MIN(tx->seg_len, shfs_size(inode) - data_of));
  if (data_len == 0) {
    return (0);
}

  buff = shbuf_init();
  shbuf_cat(buff, &blank_tx, sizeof(blank_tx));
  err = shfs_read_of(inode, buff, data_of, data_len); 
  shfs_free(&fs);
  if (err) {
fprintf(stderr, "DEBUG: %d (%s) = shfs_read_of('%s', of(%d), len(%d))\n", err, sherrstr(err), shfs_filename(inode), data_of, data_len);
    shbuf_free(&buff);
    return (err);
  }

  s_tx = (tx_file_t *)shbuf_data(buff);
  s_tx->ino_op = TXFILE_WRITE;
  s_tx->seg_of = data_of;
  s_tx->seg_len = shbuf_size(buff) - sizeof(tx_file_t);
  s_tx->seg_crc = shcrc(shbuf_data(buff) + sizeof(tx_file_t),
      shbuf_size(buff) - sizeof(tx_file_t));

  err = inittx_file(s_tx, inode);
  if (err) {
fprintf(stderr, "DEBUG: txfile_send_write: inittx_file(): err %d\n", err); 
    shbuf_free(&buff);
    return (err);
  }

  /* notify originating host */
  err = tx_send(origin, (tx_t *)s_tx);
  shbuf_free(&buff);
fprintf(stderr, "DEBUG: txfile_send_write: <%d bytes> @ %d offset [err %d]\n", s_tx->seg_len, s_tx->seg_of, err); 
  if (err)
    return (err);

#if 0
  confirm_bond_value(bond, fee);
  free_bond(&bond);
#endif

  return (0);
}

int txfile_recv_write(shpeer_t *origin, tx_file_t *tx)
{
  shstat st;
  SHFL *inode;
  shfstream_t fl;
  shfs_t *fs;
  unsigned char *raw_data;
  size_t raw_len;
  ssize_t w_len;
  int err;

/* TODO: reverify dir/file is +s'd */

  get_tx_inode(tx, &fs, &inode);

  if (0 != shfs_fstat(inode, &st)) {
    /* establish file */
    shbuf_t *buff = shbuf_init();
    shfs_write(inode, buff);
    shbuf_free(&buff);
  }

  memset(&fl, 0, sizeof(fl));
  err = shfstream_init(&fl, inode); 
  if (err)
    return (err);

  raw_data = (unsigned char *)tx + sizeof(tx_file_t);
  raw_len = tx->seg_len;
#if 0 /* DEBUG: */
  if (raw_len > MAX_CHECKSUM_SIZE)
    return (SHERR_IO);
#endif

  /* prevent any notification to remote */
  inode->blk.hdr.attr &= ~SHATTR_SYNC;

  w_len = shfstream_write(&fl, raw_data, raw_len);
  shfstream_close(&fl);

  /* determine whether file was written */
  if (w_len < 0)
    return ((int)w_len);
  if (w_len != raw_len)
    return (SHERR_IO);

  /* ensure file sync is mutual */
  inode->blk.hdr.attr |= SHATTR_SYNC;

  return (0);
}


#define SEG_CHECKSUM_SIZE 65536
/**
 * Performed when a file checksum differs between local and remote files.
 * @note only called when local file is newer then remote;
 */
int txfile_notify_segments(shpeer_t *origin, tx_file_t *tx, SHFL *inode)
{
  shbuf_t *buff;
  char data[SEG_CHECKSUM_SIZE];
  shsize_t max;
  shsize_t len;
  shsize_t of;
  int err;

/* DEBUG: todo: adjust final size upon sync */
#if 0
  /* adjust size. */
  err = shfs_truncate(inode, tx->ino.size);
  if (err)
    return (err);
#endif

  if (tx->ino_size > shfs_size(inode)) {
/* start with what we are missing, and then see if checksum matches. */
    len = MAX(0, tx->ino_size - shfs_size(inode));
fprintf(stderr, "DEBUG: txfile_notify_segments; retrieving missing data <%d bytes>\n", len); 
    if (len != 0) {
      err = txfile_send_read(origin,
          tx, inode, shfs_size(inode), len);
        if (err)
          return (err);
    } else { /* .. */ } 
#if 0
    len = MAX(0, tx->ino_size - SEG_CHECKSUM_SIZE); 
    if (len != 0) {
      err = txfile_send_read(origin, tx, inode, tx->ino_size - len, len); 
      if (err)
        return (err);
    }
#endif
  } else {
    /* validate suffix segment. */
    len = MAX(0, shfs_size(inode) - SEG_CHECKSUM_SIZE); 
    if (len != 0 && len != shfs_size(inode)) {
      err = txfile_send_read(origin, 
          tx, inode, shfs_size(inode) - len, len);
      if (err)
        return (err);
    }
  }

  return (0);
}

/**
 * An incoming TXFILE_CHECKSUM file operation from another server.
 */
int remote_file_notification(shpeer_t *origin, tx_file_t *tx)
{
  shstat st;
  shfs_t *fs;
  SHFL *inode;
  shtime_t ino_ctime;
  shtime_t ino_mtime;
  double fee;
  int ret_err;
  int err;

  ret_err = 0;

  get_tx_inode(tx, &fs, &inode);
  ino_ctime = shfs_ctime(inode);
  ino_mtime = shfs_mtime(inode);

  /* verify local inode matches remote reference. */
  if (0 != shfs_fstat(inode, &st)) {
    /* referenced file does not exist. */
    if (!(shfs_attr(shfs_inode_parent(inode)) & SHATTR_SYNC)) {
fprintf(stderr, "DEBUG: remote_file_notification: shfs_attr() !SHATTR_SYNC && 0 != shfs_fstat\n");
      /* file is only a remote reference. */
      ret_err = SHERR_REMOTE;
      goto done;
    }
  } else {
    if (!(shfs_attr(inode) & SHATTR_SYNC)) {
      /* file is not marked to be synchronized. */
      ret_err = SHERR_REMOTE;
      goto done;
    }

    if (shfs_type(inode) != SHINODE_DIRECTORY &&
        ino_ctime != tx->ino_ctime) {
fprintf(stderr, "DEBUG: remote_file_notification: !ino_ctime\n");
      /* remote reference is alternate */
      ret_err = SHERR_REMOTE;
      goto done;
    }
  }

  if (shfs_crc(inode) != tx->ino_crc) {
    /* checksums do not match. */
  
    if (tx->ino_op == TXFILE_CHECKSUM &&
        shtime_after(ino_mtime, tx->ino_mtime)) {
      /* local file is newer -- notify local file checksum */
      ret_err = txfile_send_sync(origin, tx, inode);
      goto done;
    }

    /* local time is after - we need their info */

#if 0
    fee = CALC_TXFILE_FEE(shfs_size(inode), ctime);
    if (!NO_TXFILE_FEE(fee)) {
      /* create a bond to cover net traffic expense */
/* DEBUG: */
      bond = load_bond_peer(NULL, origin, shfs_inode_peer(inode));
      if (bond && get_bond_state(bond) != TXBOND_CONFIRM) {
        free_bond(&bond);
      }
      if (!bond) {
        /* create a bond to cover net traffic expense */
        bond = create_bond_peer(origin, shfs_inode_peer(inode), 0, fee, 0.0);
        /* allow remote to confirm value [or portion of] bond. */
        set_bond_state(bond, TXBOND_CONFIRM);
      }
/* DEBUG: */
    }
#endif

    /* assign payment. */
/* DEBUG: 
    if (!bond) {
      memset(tx->ino_bond, '\000', MAX_SHARE_HASH_LENGTH);
    } else {
      strncpy(tx->ino_bond, bond->bond_tx.tx_hash, MAX_SHARE_HASH_LENGTH);
    }
*/ 

    ret_err = txfile_notify_segments(origin, tx, inode);

    //pstore_save(file, sizeof(tx_file_t));
    goto done;
  }
  
#if 0
  /* notify origin that file is synchronized. */
  send_tx = prep_tx_file(tx, TXFILE_SYNC, inode, 0, 0);
  if (send_tx) {
    tx_send(origin, send_tx);
    free(send_tx);
  }
#endif

done:
  return (ret_err);
}


#if 0 
int remote_confirm_file(tx_app_t *cli, tx_file_t *file)
{

/* ensure local file's create date is equal */

  return (0);
}
int process_file_tx(tx_app_t *cli, tx_file_t *file)
{
  tx_file_t *ent;
  int err;

  ent = (tx_file_t *)pstore_load(TX_FILE, file->ino_tx.hash);
  if (!ent) {
    /* only save if local file is to be over-written */
    err = remote_confirm_file(cli, file);
    if (err)
      return (err);
    pstore_save(file, sizeof(tx_file_t));
  }

fprintf(stderr, "DEBUG: process_file_tx: ino-op(%d) ino-path(%s)\n", file->ino_op, file->ino_path); 
  switch (file->ino_op) {
    case TXFILE_CHECKSUM:
      /* entire file checksum notification. */
      err = remote_file_notification(&cli->app_peer, file);
      if (err)
        return (err);
      break;
    case TXFILE_READ:
      /* read data segment request. */
      err = txfile_send_write(&cli->app_peer, file);
      if (err)
        return (err);
      break;
  }

  return (0);
}
#endif



static int txfile_sync_verify(tx_file_t *file)
{
  shfs_t *fs;
  shfs_ino_t *parent;
  shfs_ino_t *inode;
  shfs_attr_t attr;
  int err;

  inode = NULL;
  get_tx_inode(file, &fs, &inode);
  if (!inode)
    return (SHERR_IO);

  parent = shfs_inode_parent(inode);
  if (shfs_type(inode) == SHINODE_DIRECTORY ||
      (parent && shfs_type(parent) != SHINODE_DIRECTORY) ||
      !(shfs_attr(parent) & SHATTR_SYNC)) {
    /* verify file is present. */
    err = shfs_fstat(inode, NULL);
    if (err) {
      shfs_free(&fs);
      return (err);
    }
  }

  attr = shfs_attr(inode);
  if (shfs_format(inode) == SHINODE_FILE &&
      (attr & SHATTR_SYNC) &&
      shfs_ctime(inode) != file->ino_ctime) {
    /* remote file reference. */
    shfs_free(&fs);
    return (SHERR_REMOTE);
  }

  /* obtain file attributes */
  shfs_free(&fs);
  return (0);
}


int txop_file_init(shpeer_t *cli, tx_file_t *file)
{
  shbuf_t *buff;
  shfs_t *fs;
  shfs_ino_t *inode;
  int err;

  /* set default initial operation */
  if (file->ino_op == TXFILE_NONE)
    file->ino_op = TXFILE_CHECKSUM; 

  /* update checksum and last-modified timestamp */
  get_tx_inode(file, &fs, &inode);
  file->ino_mtime = shfs_mtime(inode);
  file->ino_crc = shfs_crc(inode);
  shfs_free(&fs);

  return (0);
}

int txop_file_confirm(shpeer_t *cli, tx_file_t *file)
{
  int err;

  err = txfile_sync_verify(file);
  if (err)
    return (err);

  return (0);
}

int txop_file_recv(shpeer_t *cli, tx_file_t *file)
{
  int err;

//fprintf(stderr, "DEBUG: txop_file_recv: ino_op %d (<%d bytes>)\n", file->ino_op, file->ino_size);
  switch (file->ino_op) {
    case TXFILE_CHECKSUM:
    case TXFILE_SYNC:
      /* entire file checksum notification. */
      err = remote_file_notification(cli, file);
      if (err)
        return (err);
      break;
    case TXFILE_READ:
      /* read data segment request. */
      err = txfile_send_write(cli, file);
      if (err)
        return (err);
      break;
    case TXFILE_WRITE:
      /* segment of file data notification. */
      err = txfile_recv_write(cli, file);
      if (err)
        return (err);
      break;
  }

  return (0);
}

int txop_file_send(shpeer_t *cli, tx_file_t *file)
{

  return (0);
}



int inittx_file(tx_file_t *file, shfs_ino_t *inode)
{
  shfs_t *fs;
  shfs_ino_t *parent;
  tx_file_t *send_tx;
  shbuf_t *buff;
  shstat st;
  char sig_hash[MAX_SHARE_HASH_LENGTH];
  int err;

memset(&st, 0, sizeof(st));

  parent = shfs_inode_parent(inode);
  if (!parent || 
      shfs_type(parent) != SHINODE_DIRECTORY ||
      !(shfs_attr(parent) & SHATTR_SYNC)) {
    err = shfs_fstat(inode, &st);
    if (err) {
fprintf(stderr, "DEBUG: inittx_file: %d = shfs_fstat()\n", err);
      return (err);
}
  }

  set_tx_inode(file, inode);

  err = tx_init(NULL, (tx_t *)file, TX_FILE);
  if (err)
    return (err);

  return (0);
}

tx_file_t *alloc_file(shfs_ino_t *inode)
{
  tx_file_t *file;
  int err;

  file = (tx_file_t *)calloc(1, sizeof(tx_file_t));
  if (!file)
    return (NULL);

  err = inittx_file(file, inode);
  if (err) {
    PRINT_ERROR(err, "alloc_file [initialization]");
    return (NULL);
  }

  return (file);
}

tx_file_t *alloc_file_path(shpeer_t *peer, char *path)
{
  shstat st;
  tx_file_t *ret_file;
  shfs_t *fs;
  shfs_ino_t *inode;
char uri[PATH_MAX+1];
  int err;

  if (!path)
    return (NULL);

  if (!peer)
    peer = ashpeer();

  ret_file = NULL;
  sprintf(uri, "%s:%s", shpeer_get_app(peer), path);
  fs = shfs_uri_init(uri, 0, &inode);
  if (!fs)
    return (ret_file);

  err = shfs_fstat(inode, &st);
  if (err) {
    shfs_free(&fs);
    return (ret_file);
  }

  ret_file = alloc_file(inode);
  shfs_free(&fs);

  return (ret_file);
}



