
sync_sources = \
	sync/sync.c \
	sync/sync.h \
	sync/sync_share.c \
	sync/sync_share.h \
	sync/sync_linux.c \
	sync/sync_linux.h

common_headers = \
	fsync_pref.h \
	fsync_user.h \
	fsync_server.h \
	fsync_cycle.h

common_sources = \
	fsync_pref.c \
	fsync_user.c \
	fsync_server.c \
	fsync_cycle.c



if HAVE_FSYNC_DAEMON
sbin_PROGRAMS = shfsyncd

shfsyncd_SOURCES = $(common_headers) $(common_sources) $(sync_sources)
shfsyncd_CFLAGS = -DSHARE_PUB_DAEMON \
								-I$(top_srcdir)/src/share-lib \
								-I$(top_srcdir)/src/sexe \
								-I$(top_srcdir)/src/share-daemon/bits \
								-lcrypt -O3
endif

## noinst_PROGRAMS = shfsyncd_static
## shfsyncd_static_SOURCES = $(common_headers) $(common_sources) $(sync_sources)
## shfsyncd_static_LDADD = -lrt
## #$(top_builddir)/src/gnu/libgnu.a -lrt
## shfsyncd_static_CFLAGS = -DSHARE_PUB_DAEMON \
## 								-I$(top_srcdir)/src/share-lib \
## 								-I$(top_srcdir)/src/sexe \
## 								-I$(top_srcdir)/src/share-daemon/bits \
## 								-lcrypt -g

