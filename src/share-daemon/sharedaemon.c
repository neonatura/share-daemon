
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
#include "sharedaemon.h"
#include <sys/types.h>
#include <signal.h>
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#include <stddef.h>

static int _no_fork;

shpeer_t *server_peer;
tx_ledger_t *server_ledger;
int shlogd_pid;
int server_port;
int http_server_port;


void sharedaemon_term(void)
{
  int err;

  sharedaemon_bcast_term();

  sharedaemon_client_term();

  if (listen_sk) {
    shclose(listen_sk);
    listen_sk = 0;
  }

#ifdef SHARED_API_SERVICE
  if (http_listen_sk) {
    shclose(http_listen_sk);
    http_listen_sk = 0;
  }
#endif

  cycle_term();
  shpeer_free(&server_peer);
}

void sharedaemon_signal(int sig_num)
{
  signal(sig_num, SIG_DFL);

  if (shlogd_pid)
    kill(shlogd_pid, SIGQUIT);

  sharedaemon_term();
  raise(sig_num);

}

static void sharedaemon_print_usage(void)
{

  printf(
    "shared: A libshare suite transaction daemon.\n"
    "\n"
    "Usage: shared [OPTION]\n"
    "\n"
    "Options:\n"
    "\t[-p|-port]\tThe shared daemon TCP listening port.\n"
#ifdef SHARED_API_SERVICE
    "\t[-nh|--no-http]\tDisable the shared HTTP API listening port.\n"
#endif
    "\t[-nf|--no-fork]\tRun daemon as a foreground process.\n"
    "\n"
    "Visit http://docs.sharelib.net for additional documentation.\n"
    "Report bugs to http://bugs.sharelib.net/ or <support@neo-natura.com>.\n"
    );
}

static void sharedaemon_print_version(void)
{

  printf(
    "shared version %s\n"
    "\n"
    "Copyright 2013 Neo Natura\n"
    "Licensed under the GNU GENERAL PUBLIC LICENSE Version 3\n",
    get_libshare_version()
    );
}

int main(int argc, char *argv[])
{
	shpeer_t *peer;
  char buf[256];
  int fd_max;
  int err;
  int fd;
  int i;

  server_port = SHARE_DAEMON_PORT;

#ifdef SHARED_API_SERVICE
  http_server_port = SHARE_HTTP_DAEMON_PORT;
#endif

  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "--version") ||
        0 == strcmp(argv[i], "-v")) {
      sharedaemon_print_version();
      return (0);
    }
    if (0 == strcmp(argv[i], "--help") ||
        0 == strcmp(argv[i], "-h")) {
      sharedaemon_print_usage();
      return (0);
    }
    if (0 == strcmp(argv[i], "-nf")) {
      _no_fork = TRUE;
    } else if (0 == strcmp(argv[i], "-p") ||
        0 == strcmp(argv[i], "--port")) {
      int port;
      if (i + 1 < argc) {
        port = atoi(argv[i+1]);
        if (port)
          server_port = port; 
      }
#ifdef SHARED_API_SERVICE
    } else if (0 == strcmp(argv[i], "-nh") ||
        0 == strcmp(argv[i], "--no-http")) {
      http_server_port = 0;
#endif
    }
  }


#ifdef ENABLE_SHLOG_DAEMON
  /* fork shlogd */
  if (0 == strcmp(shpref_get("daemon.shlogd", SHPREF_TRUE), SHPREF_TRUE)) {
    shlogd_pid = fork();
    switch (shlogd_pid) {
      case 0:
        /* shlogd process */
        memset(argv[0], '\0', strlen(argv[0]));
        strcpy(argv[0], "shlogd");
        return (shlogd_main(argc, argv));
      case -1:
        sherr(-errno, "shlogd spawn");
        break;
      default:
        sprintf(buf, "spawned shlogd daemon (pid %d)", shlogd_pid);
        shinfo(buf);
        break;
    }
  }
#endif

  if (!_no_fork) {
    daemon(0, 1);
  }

  signal(SIGHUP, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGTERM, sharedaemon_signal);
  signal(SIGQUIT, sharedaemon_signal);
  signal(SIGINT, sharedaemon_signal);

  fd = shnet_sk();
  if (fd == -1) {
    perror("shsk");
    return (-1);
  }
  
  err = shnet_bindsk(fd, NULL, server_port);
  if (err) {
    perror("shbindport (server)");
    shclose(fd);
    return (err);
  }

  listen_sk = fd;


#ifdef SHARED_API_SERVICE
  if (http_server_port) {
    fd = shnet_sk();
    if (fd == -1) {
      perror("shsk");
      return (-1);
    }

    err = shnet_bindsk(fd, NULL, http_server_port);
    if (err) {
      perror("shbindport (http)");
      shclose(listen_sk);
      shclose(fd);
      return (err);
    }

    fd_max = (int)shproc_rlim(RLIMIT_NOFILE) / 20; /* 5% */
    fd_max = MAX(50, fd_max);
    if (fd_max < SOMAXCONN)
      shnet_listen(fd, fd_max);

    http_listen_sk = fd;
  }
#endif


  server_ledger = (tx_ledger_t *)calloc(1, sizeof(tx_ledger_t));

	server_peer = shapp_init("shared", NULL, SHAPP_LOCAL);

  sharedaemon_bcast_send();

//	server_account = sharedaemon_account();
//	printf ("Using server account '%s'.\n", server_account->hash);
//	printf ("Using user identity '%s'.\n", server_account->id.hash);

  share_server(argv[0], PROC_SERVE);

  shpeer_free(&server_peer);


  return (0);
}


