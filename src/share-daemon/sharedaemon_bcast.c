
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

#include "share.h"
#include "sharedaemon.h"
#include <ifaddrs.h>

/*
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
*/

static int _bcast_recv_fd;
static struct sockaddr_in _bcast_recv_addr;

#define SHARED_BROADCAST_PORT SHARE_DAEMON_PORT 
#define BROADCAST_TIMEOUT 16

int bcast_send_init(void)
{
  struct sockaddr_in addr; /* todo: ipv6 */
  int so_reuseaddr = TRUE;
  int so_broadcast = TRUE;
  int err;
  int fd;

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1)
    return (-errno);

  err = setsockopt(fd, SOL_SOCKET, SO_BROADCAST,
      &so_broadcast, sizeof(so_broadcast));
  if (err) {
fprintf(stderr, "DEBUG: setsockoptSO_BROADCAST fail %s\n", strerror(errno));
    close(fd);
    return (-errno);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = (in_port_t)htons(SHARED_BROADCAST_PORT);
  addr.sin_addr.s_addr = INADDR_ANY;
  err = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (err) {
fprintf(stderr, "DEBUG: bcast_init/send: bind fail: fd(%d) INADDR_ANY errno(%s)\n", fd, strerror(errno));
    close(fd);
    return (-errno);
  }

  return (fd);
}

int bcast_recv_init(void)
{
  struct sockaddr_in addr; /* todo: ipv6 */
  int so_reuseaddr = TRUE;
  int so_broadcast = TRUE;
  int err;
  int fd;

  if (!_bcast_recv_fd) {
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
      return (-errno);

    err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
        &so_reuseaddr, sizeof(so_reuseaddr));
    if (err) {
      close(fd);
      return (-errno);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SHARED_BROADCAST_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    err = bind(fd, &addr, sizeof(addr));
    if (err) {
      close(fd);
      return (-errno);
    }

    _bcast_recv_fd = fd;
    memcpy(&_bcast_recv_addr, &addr, sizeof(struct sockaddr_in));
  }


  return (0);
}

int sharedaemon_bcast_recv(void)
{
  struct sockaddr_in addr;
  socklen_t addr_len;
  struct timeval to;
  fd_set read_set;
  shpeer_t *peer;
  char dgram[512];
  ssize_t r_len;
  int err;

  err = bcast_recv_init();
  if (err) {
    return (err);
}

  FD_ZERO(&read_set);
  FD_SET(_bcast_recv_fd, &read_set);

  /* nonblocking read */
  memset(&to, 0, sizeof(to));
  err = select(_bcast_recv_fd+1, &read_set, NULL, NULL, &to); 
  if (err < 0) {
    return (-errno);
}
  if (err == 0) {
//fprintf(stderr, "\rWaiting for select(_bcast_recv_fd)..");
//fflush(stderr);
    return (0); /* nothing to read */
}

  addr_len = sizeof(addr);
  memset(&addr, 0, addr_len);
  r_len = recvfrom(_bcast_recv_fd, 
      dgram, sizeof dgram, 0, &addr, &addr_len);
  if (r_len < 0) {
fprintf(stderr, "DEBUG: %d = recvfrom()\n", r_len);
    return (-errno);
  }

  /* and who are you? */
  if (r_len < sizeof(shpeer_t)) {
fprintf(stderr, "DEBUG: <%d bytes> pending..\n", r_len);
    return (SHERR_INVAL);
  }

#if 0
  now = shtime();
  tx = (tx_t *)dgram;
  if (shtime_after(tx->tx_stamp, now) ||
      shtime_before(tx->tx_stamp, shtime_adj(now, -BROADCAST_TIMEOUT))) {
    /* broadcast message must indicate sane time-frame. */
    return (SHERR_TIME);
  }

  switch (tx->tx_op) {
    case TX_PEER:
      peer_tx = (tx_peer_t *)dgram;
      if (0 != shkey_cmp(&tx->tx_peer, shpeer_kpriv(&peer_tx->peer)))
        return (SHERR_INVAL); /* only accept self-referencing broadcast */
  }
#endif


  /* share-daemon broadcasting it's peer address. */
  peer = (shpeer_t *)dgram;

  if (!shkey_cmp(shpeer_kpub(sharedaemon_peer()), shpeer_kpub(peer))) {
    /* this is not a shared peer */
    return (0); /* all done */
  }

  if (!shkey_cmp(shpeer_kpub(sharedaemon_peer()), shpeer_kpub(peer))) {
fprintf(stderr, "DEBUG: invalid key\n");
    /* this is a peer referencing ourselves. */
      //err = sharedaemon_netclient_alias(&addr);
  }



  switch (peer->type) {
    case SHNET_PEER_LOCAL:
    case SHNET_PEER_IPV4:

      /*
         memset(&addr, '\000', sizeof(struct sockaddr_in));
         memcpy(&addr, &peer_tx->peer.addr, sizeof(peer_tx->peer.addr));
         */
      fprintf(stderr, "DEBUG: received UDP broadcast with peer \"%s\"\n", shpeer_print(peer));
      fprintf(stderr, "DEBUG: received UDP broadcast for \"%s\" port %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
      if (!peer->addr.sin_port)
        break; /* otay */

      addr.sin_family = AF_INET;
      err = sharedaemon_netclient_conn(peer, &addr);
      if (err)
        return (err);

      break;
  }
fprintf(stderr, "DEBUG: processed bcast recv\n");

  return (0);
}

int sharedaemon_bcast_send_peer(shpeer_t *peer)
{
  struct sockaddr_in addr;
  socklen_t addr_len;
  struct timeval to;
  fd_set write_set;
  char dgram[512];
  ssize_t w_len;
  int fd;
  int err;

  fd = bcast_send_init();
  if (fd < 0) {
fprintf(stderr, "DEBUG: sharedaemon_bcast_send: bcast_init error %d\n", err);
    return (err);
  }


  FD_ZERO(&write_set);
  FD_SET(fd, &write_set);

  /* nonblocking write */
  memset(&to, 0, sizeof(to));
  err = select(fd+1, NULL, &write_set, NULL, &to); 
  if (err < 0) {
close(fd);
    return (-errno);
}
  if (err == 0) {
close(fd);
    return (0); /* not able to send */
}

  memset(dgram, 0, sizeof(dgram));
  memcpy(dgram, peer, sizeof(shpeer_t));

  addr_len = sizeof(addr);
  memset(&addr, 0, addr_len);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  addr.sin_port = htons(SHARED_BROADCAST_PORT);
  w_len = sendto(fd,
      dgram, sizeof(shpeer_t), 0, &addr, sizeof(addr));
  if (w_len < 0) {
fprintf(stderr, "DEBUG: sharedaemon_bcast_send: sendto error: %s\n", strerror(errno));
    close(fd);
    return (-errno);
}

fprintf(stderr, "DEBUG: sharedaemon_bcast_send: peer \"%s\" <%d bytes>\n", shpeer_print(peer), w_len);
fprintf(stderr, "DEBUG: peer pubkey: %s\n", shkey_print(shpeer_kpub(peer)));
fprintf(stderr, "DEBUG: peer priv key: %s\n", shkey_print(shpeer_kpriv(peer)));


  /* close socket; only used once. */
  close(fd);

  return (0);
}

int sharedaemon_bcast_send(void)
{
  struct ifaddrs *if_list;
  struct ifaddrs *dev;
  shpeer_t *peer;
  char hostname[NI_MAXHOST+1];
  int err;

  err = getifaddrs(&if_list);
  if (err)
    return (-errno);

  /* cycle through all non loop-back interfaces. */
  for (dev = if_list; dev; dev = dev->ifa_next) {
    if (dev->ifa_addr == NULL)
      continue;

    err = SHERR_OPNOTSUPP;

    memset(hostname, 0, sizeof(hostname));
    switch (dev->ifa_addr->sa_family) {
      case AF_INET:
        err = getnameinfo(dev->ifa_addr, sizeof(struct sockaddr_in),
            hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (err)
          break;

        if (0 == strncmp(hostname, "127.0.0.", strlen("127.0.0."))) {
          /* local loop-back */
          err = SHERR_AGAIN;
          break;
        }

        fprintf(stderr, "DEBUG: found inet device '%s' with addr '%s'\n", dev->ifa_name, hostname);

        err = 0;
        break;

      case AF_INET6:
        err = getnameinfo(dev->ifa_addr, sizeof(struct sockaddr_in6),
            hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (err)
          break;

        if (0 == strcmp(hostname, "::1")) {
          /* local loop-back */
          err = SHERR_AGAIN;
          break;
        }
        fprintf(stderr, "DEBUG: found inet6 device '%s' with addr '%s'\n", dev->ifa_name, hostname);


        err = 0;
        break;

      default:
        fprintf(stderr, "DEBUG: found unknown (fam %d) device '%s' with addr '%s'\n", dev->ifa_addr->sa_family, dev->ifa_name, hostname);
        break;
    }
    if (err) {
      /* .. */
      continue;
    }
    
    sprintf(hostname + strlen(hostname), " %d", server_port);
    peer = shpeer_init("shared", hostname);
fprintf(stderr, "DEBUG: sharedaemon_bcast_send: %d = sharedaemon_bcast_send_peer(\"%s\")\n", err, hostname); 
    err = sharedaemon_bcast_send_peer(peer);
    shpeer_free(&peer);
    if (err) {
      /* .. */
    }
  }

  return (0);
}

void sharedaemon_bcast_term(void)
{
  if (_bcast_recv_fd) {
    close(_bcast_recv_fd);
    _bcast_recv_fd = 0;
  }
}
