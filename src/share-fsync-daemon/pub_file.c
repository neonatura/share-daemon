
#include "pub_server.h"



shmap_t *_pubd_file_map;

void pubd_file_init(void)
{
  _pubd_file_map = shmap_init();
}
void pubd_file_free(void)
{
  shmap_free(&_pubd_file_map);
}

shfs_ino_t *pubd_shfs_path(pubuser_t *u, char *path)
{
  static char fs_path[PATH_MAX+1];
  shfs_ino_t *file;

  if (shkey_cmp(&u->id, ashkey_blank())) {
    return (NULL);
  }

  if (!u->fs) {
    u->fs = shfs_home_fs(&u->id);
  }

  memset(fs_path, 0, sizeof(fs_path));
  if (0 == strncmp(path, u->root_path, strlen(u->root_path)))
    strncpy(fs_path, path + strlen(u->root_path), sizeof(fs_path) - 1);
  else
    strncpy(fs_path, path, sizeof(fs_path) - 1);

  file = shfs_home_file(u->fs, fs_path);
fprintf(stderr, "DEBUG: #%x (%s) = pubd_shfs_path('%s')\n", file, fs_path, path); 
return (file);
}

int pubd_file_sync(pubuser_t *u, pubfile_t *f, SHFL *fl)
{
  struct utimbuf ti;
  struct stat st;
  int err;

  err = shfs_fstat(fl, &st);
fprintf(stderr, "DEBUG: SYNC: pubd_file_sync: %d = shfs_fstat(): <%d bytes>\n", err, st.st_size);
  if (err)
    return (err);

  ti.actime = st.st_atime;
  ti.modtime = st.st_mtime;
  err = utime(f->path, &ti);
  if (err)
    return (err);

  f->stamp = st.st_mtime;
  f->size = st.st_size;
  f->stat.sync_tot++;

  return (0);
}

int pubd_file_upload(pubuser_t *u, pubfile_t *f, char *path, time_t stamp)
{
  struct stat st;
  SHFL *fl;
  shbuf_t *buff;
  unsigned char *data;
  size_t data_len;
  int err;

fprintf(stderr, "DEBUG: pubd_file_upload: %s\n", path);

  fl = pubd_shfs_path(u, path);

#if 0
  err = shfs_fstat(fl, &st);
  if (err)
    return (err); 
#endif
  
#if 0
  if (st.st_mtime > stamp) {
/* DEBUG: todo: convert to shfs_dir_list.. */
    f->stamp = st.st_mtime;
    return (0); /* shnet has fresher data */
  }
#endif

  err = shfs_read_mem(path, (char **)&data, &data_len);
  if (err)
    return (err);

  buff = shbuf_init();
  shbuf_cat(buff, data, data_len);
  err = shfs_write(fl, buff); 
  shbuf_free(&buff);
  if (err)
    return (err);

  pubd_file_sync(u, f, fl);

fprintf(stderr, "DEBUG: pubd_file_upload: wrote %d bytes to shfs '%s' from '%s'\n", data_len, shfs_filename(fl), path);

  return (0);
}

int pubd_file_download(pubuser_t *u, pubfile_t *f, char *path)
{
  struct stat st;
  SHFL *fl;
  shbuf_t *buff;
  unsigned char *data;
  size_t data_len;
  int err;

fprintf(stderr, "DEBUG: pubd_file_download: %s\n", path);

  fl = pubd_shfs_path(u, path);
  err = shfs_fstat(fl, &st);
  if (err)
    return (err);

  if (f->stamp == st.st_mtime)
    return (0); /* nothing changed */

  buff = shbuf_init();
  err = shfs_read(fl, buff);
  if (err) {
    shbuf_free(&buff);
    return (err);
  }

  err = shfs_write_mem(path, shbuf_data(buff), shbuf_size(buff));
  shbuf_free(&buff);
  if (err)
    return (err);

fprintf(stderr, "DEBUG: pubd_file_upload: wrote %d bytes to disk path '%s'\n", data_len, path);

  pubd_file_sync(u, f, fl);

  return (0);
}


void pubd_file_verify(pubuser_t *u, char *path)
{
  pubfile_t *f;
  struct stat st;
  shkey_t *key;
  int err;
  
  err = stat(path, &st);
fprintf(stderr, "DEBUG: pubd_file_verify: %d = stat(%s)\n", err, path);
  if (err)
    return;

  if (!S_ISREG(st.st_mode))
    return;

  key = shkey_str(path);

  f = (pubfile_t *)shmap_get_ptr(_pubd_file_map, key);
  if (!f) {
    f =  (pubfile_t *)calloc(1, sizeof(pubfile_t));
    strncpy(f->path, path, sizeof(f->path) - 1);
    shmap_set_ptr(_pubd_file_map, key, f);
  }

  if (f->stamp != st.st_mtime) {
    /* file changed in home dir */
    pubd_file_upload(u, f, path, st.st_mtime); 
  } else {
    time_t now = time(NULL);
    if (f->scan_t < (now - MAX_PUBFILE_REFRESH_TIME)) {
      pubd_file_download(u, f, path);
      f->scan_t = now;
    }
  }

  shkey_free(&key);

}

