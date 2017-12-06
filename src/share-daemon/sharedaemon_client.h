

#ifndef __SHAREDAEMON_CLIENT_H__
#define __SHAREDAEMON_CLIENT_H__


/* client is using shared protocol via message queue. */
#define SHD_CLIENT_MSG (1 << 0)
/* client is using shared protocol via network connection. */
#define SHD_CLIENT_NET (1 << 1)
/* client is using http protocol. */
#define SHD_CLIENT_HTTP (1 << 2)
/* client is registered as external shared node. */
#define SHD_CLIENT_REGISTER (1 << 3)
/* client requires authorization to be registered */
#define SHD_CLIENT_AUTH (1 << 4)
/* shutdown socket after connection. */
#define SHD_CLIENT_SHUTDOWN (1 << 5)
#define SHD_CLIENT_POST (1 << 6)
#define SHD_CLIENT_DISPOSITION (1 << 7)

/** Request feedback on a particular operation mode. */
#define SHOP_LISTEN (1 << 0)


#define TEMPL_DEFAULT 0
#define TEMPL_REGISTER 1
#define TEMPL_LOGIN 2
#define TEMPL_ACCESS 3
#define TEMPL_2FA 4


/* deprec sock_t & move to..*/
typedef struct shd_net_t {
  int fd;

  /* daemon */
  int ini_seq;

  /* http */
  char tmpl[256];
  shmap_t *fields;

  /** The next time a clock transaction will be performed. */
  shtime_t clock_stamp;
  /** The running average of the clock offset between two hosts. */
  devclock_t clock;
} shd_net_t;

typedef struct shd_msg_t {
  shkey_t msg_key;
  shpeer_t app_peer;
} shd_msg_t;

#if 0
/**
 * Specifies which peer operations that the client has requested to be informed on.
 */
typedef struct shd_listen_t {
  shtime_t li_stamp;
  shkey_t li_key;
  int li_op;

  struct shd_listen_t *next;
} shd_listen_t;
#endif

typedef struct shd_t {
  int flags;
  shpeer_t peer;
  shtime_t birth;
  tx_app_t app;

  union {
    shd_net_t net;
    shd_msg_t msg;
  } cli;

  /** Behaviour specification for particular operation modes. */
  uint8_t op_flags[MAX_TX];

  /** Specific key-sets client has subscribed to. */
  shmap_t *listen_map;

  /* incoming & outgoing data buffers. */
  shbuf_t *buff_out;
  shbuf_t *buff_in;
  shtime_t buff_stamp;

  struct shd_t *next;
} shd_t;

extern shd_t *sharedaemon_client_list;

shd_t *sharedaemon_client_find(shkey_t *key);

int sharedaemon_msgclient_init(shpeer_t *peer);

int sharedaemon_netclient_conn(shpeer_t *net_peer, struct sockaddr_in *net_addr);

int sharedaemon_netclient_init(int fd, struct sockaddr *net_addr);

int sharedaemon_client_count(struct sockaddr *net_addr);

int sharedaemon_netclient_add(int fd, shpeer_t *peer, int flags);

int sharedaemon_httpclient_add(int fd);

int sharedaemon_client_listen(shd_t *cli, tx_subscribe_t *sub);


#endif /* ndef __SHAREDAEMON_CLIENT_H__ */


