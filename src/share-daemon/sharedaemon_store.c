
#include "share.h"
#include "sharedaemon.h"

shfs_t *_pstore_fs;

void pstore_init(void)
{
  _pstore_fs = sharedaemon_fs();
}

void *pstore_read(int tx_op, char *name)
{
  SHFL *fl;
  shbuf_t *buff;
  char path[PATH_MAX+1];
  char prefix[256];
  unsigned char *data;
  size_t data_len;
  int err;

  pstore_init(); 

  memset(prefix, 0, sizeof(prefix));
  switch (tx_op) {
    case TX_ACCOUNT:
      strcpy(prefix, "account");
      break;
    case TX_IDENT:
      strcpy(prefix, "id");
      break;
    case TX_SESSION:
      strcpy(prefix, "session");
      break;
    case TX_APP:
      strcpy(prefix, "app");
      break;
    case TX_LEDGER:
      strcpy(prefix, "ledger");
      break;
    case TX_CONTEXT:
      strcpy(prefix, "ctx");
      break;
    default:
      strcpy(prefix, "default");
      break;
  }

  buff = shbuf_init();
  sprintf(path, "/sys/net/tx/%s/%s", prefix, name);
  fl = shfs_file_find(_pstore_fs, path);
  err = shfs_read(fl, buff);
  if (err) {
fprintf(stderr, "DEBUG: unknown pstore load file '%s' [%s]\n", name, prefix);
    shbuf_free(&buff);
    return (NULL);
  }

  return (shbuf_unmap(buff));
}

int pstore_write(int tx_op, char *name, unsigned char *data, size_t data_len)
{
  SHFL *fl;
  char prefix[256];
  char path[PATH_MAX+1];
  shbuf_t *buff;
  int err;

  pstore_init(); 

  memset(prefix, 0, sizeof(prefix));
  switch (tx_op) {
    case TX_ACCOUNT:
      strcpy(prefix, "account");
      break;
    case TX_IDENT:
      strcpy(prefix, "id");
      break;
    case TX_SESSION:
      strcpy(prefix, "session");
      break;
    case TX_APP:
      strcpy(prefix, "app");
      break;
    case TX_LEDGER:
      strcpy(prefix, "ledger");
      break;
    case TX_CONTEXT:
      strcpy(prefix, "ctx");
      break;
    default:
      strcpy(prefix, "default");
      break;
  }

  sprintf(path, "/sys/net/tx/%s/%s", prefix, name);
  fl = shfs_file_find(_pstore_fs, path);
buff = shbuf_init();
shbuf_cat(buff, data, data_len);
  err = shfs_write(fl, buff);
shbuf_free(&buff);
fprintf(stderr, "DEBUG: pstore_write: %d = shfs_write('%s') [%s]\n", err, name, prefix);
  if (err)
    return (err);

  return (0);
}

int pstore_save(void *data, size_t data_len)
{
  tx_t *tx;
  shkey_t *s_key;

  tx = (tx_t *)data;

  s_key = get_tx_key(tx);
  if (!s_key)
    return (SHERR_INVAL);

  pstore_write(tx->tx_op, (char *)shkey_hex(s_key),
      (unsigned char *)data, data_len);

  return (0);
}

int pstore_save_tx(tx_t *tx)
{
  unsigned char *data = (unsigned char *)tx;
  size_t data_len;

  data_len = get_tx_size(tx);
  if (!data_len)
    return (SHERR_INVAL);

  return (pstore_save(data, data_len));
}

void *pstore_load(int tx_op, char *hash)
{
  return (pstore_read(tx_op, hash));
}

int pstore_delete(int tx_op, char *hash)
{
  char path[PATH_MAX+1];
  char prefix[256];
  shfs_ino_t *fl;
  int err;

  pstore_init(); 

  memset(prefix, 0, sizeof(prefix));
  switch (tx_op) {
    case TX_ACCOUNT:
      strcpy(prefix, "account");
      break;
    case TX_IDENT:
      strcpy(prefix, "id");
      break;
    case TX_SESSION:
      strcpy(prefix, "session");
      break;
    case TX_APP:
      strcpy(prefix, "app");
      break;
    case TX_LEDGER:
      strcpy(prefix, "ledger");
      break;
    case TX_LICENSE:
      strcpy(prefix, "license");
      break;
    case TX_CONTEXT:
      strcpy(prefix, "ctx");
      break;
    default:
      strcpy(prefix, "default");
      break;
  }

  sprintf(path, "/sys/net/tx/%s/%s", prefix, hash);
  fl = shfs_file_find(_pstore_fs, path);
  err = shfs_file_remove(fl);
  if (err)
    return (err);

  return (0);
}

int pstore_delete_tx(tx_t *tx)
{
  shkey_t *s_key;
  int err;
  
  if (!tx)
    return (SHERR_INVAL);

  s_key = get_tx_key(tx);
  if (!s_key)
    return (SHERR_INVAL);

  err = pstore_delete(tx->tx_op, (char *)shkey_hex(s_key));
  if (err)
    return (err);

  return (0);
}

void pstore_free(void *tx)
{

  if (tx)
    free(tx);

}


