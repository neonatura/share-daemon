AUTOMAKE_OPTIONS = foreign

TESTS = txtest

if HAVE_LIBUSB

USBLIB=-lusb-1.0

else

libusb_sources = \
	dev/usb/core.c \
	dev/usb/descriptor.c \
	dev/usb/io.c \
	dev/usb/libusb.h \
	dev/usb/libusbi.h \
	dev/usb/sync.c \
	dev/usb/version.h
if WINDOWS
libusb_sources += \
	dev/usb/windows_usb.c \
	dev/usb/windows_usb.h \
	dev/usb/threads_windows.c \
	dev/usb/threads_windows.h \
	dev/usb/poll_windows.c \
	dev/usb/poll_windows.h
libusb_flags = -DOS_WINDOWS
endif
if LINUX
libusb_sources += \
	dev/usb/threads_posix.c \
	dev/usb/threads_posix.h \
	dev/usb/linux_usbfs.c \
	dev/usb/linux_usbfs.h \
	dev/usb/poll_posix.h
libusb_flags = -DOS_LINUX -DTHREADS_POSIX -I$(srcdir)/dev/usb
endif
if FREEBSD
libusb_sources += \
	dev/usb/openbsd_usb.c
libusb_flags = -DOS_LINUX -I$(srcdir)/dev/usb
endif

noinst_LIBRARIES = libusb.a
libusb_a_SOURCES = $(libusb_sources)
libusb_a_CFLAGS = $(libusb_flags)

noinst_PROGRAMS = txtest 

USBLIB=libusb.a

endif

common_INCLUDES = \
	$(top_builddir)/config.h \
	$(top_srcdir)/src/share-lib/share.h \
	$(top_srcdir)/src/share-lib/mem/shmem.h \
	$(top_srcdir)/src/share-lib/sherr.h \
	$(top_srcdir)/src/share-lib/share_base.h \
	$(top_srcdir)/src/share-lib/shpeer.h \
	$(top_srcdir)/src/share-lib/shtime.h \
	$(top_srcdir)/src/share-lib/shpref.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/shfile.h \
	$(top_srcdir)/src/share-lib/fs/shfs.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3.h \
	$(top_srcdir)/src/share-lib/net/shnet.h \
	sharedaemon.h \
	sharedaemon_server.h \
	sharedaemon_file.h \
	sharedaemon_client.h \
	sharedaemon_device.h \
	sharedaemon_app.h \
	sharedaemon_store.h \
	sharedaemon_bcast.h \
	sharedaemon_peer.h \
	bits/bits.h \
	bits/app.h \
	bits/transaction.h \
	bits/event.h \
	bits/signature.h \
	bits/metric.h \
	bits/run.h \
	bits/vm.h \
	bits/license.h \
	bits/trust.h \
	bits/account.h \
	bits/identity.h \
	bits/session.h \
	bits/ledger.h \
	bits/wallet.h \
	bits/schedule.h \
	bits/ward.h \
	bits/clock.h \
	bits/eval.h \
	bits/context.h \
	bits/asset.h \
	bits/file.h \
	bits/init.h \
	bits/subscribe.h \
	dev/dev.h \
	dev/dev_clock.h \
	dev/clock_local.h \
	dev/ntp_proto.h \
	dev/card_usb.h \
	dev/card_kmap.h \
	dev/fpga_usb.h \
	dev/leitch_serial.h \
	http/oauth_response.h \
	http/oauth_session.h \
	http/oauth_admin.h \
	http/oauth_api.h \
	http/oauth_2fa.h \
	http/oauth_db.h \
	http/oauth_template.h \
	http/oauth_favicon.h \
	http/api_exec.h \
	http/api_user.h \
	http/api_media.h \
	http/api_geo.h \
	http/api_crypt.h

common_SOURCES = \
	sharedaemon.c \
	sharedaemon_client.c \
	sharedaemon_device.c \
	sharedaemon_server.c \
	sharedaemon_app.c \
	sharedaemon_store.c \
	sharedaemon_file.c \
	sharedaemon_bcast.c \
	sharedaemon_peer.c \
	bits/bits.c \
	bits/app.c \
	bits/transaction.c \
	bits/event.c \
	bits/signature.c \
	bits/metric.c \
	bits/run.c \
	bits/vm.c \
	bits/license.c \
	bits/trust.c \
	bits/account.c \
	bits/identity.c \
	bits/session.c \
	bits/ledger.c \
	bits/wallet.c \
	bits/schedule.c \
	bits/ward.c \
	bits/clock.c \
	bits/eval.c \
	bits/reference.c \
	bits/context.c \
	bits/asset.c \
	bits/file.c \
	bits/init.c \
	bits/subscribe.c \
	dev/dev_clock.c \
	dev/dev_usb.c \
	dev/clock_local.c \
	dev/card_usb.c \
	dev/card_kmap.c \
	dev/fpga_usb.c \
	dev/leitch_serial.c \
	http/oauth.c \
	http/oauth_response.c \
	http/oauth_session.c \
	http/oauth_admin.c \
	http/oauth_api.c \
	http/oauth_2fa.c \
	http/oauth_db.c \
	http/oauth_template.c \
	http/api_exec.c \
	http/api_user.c \
	http/api_media.c \
	http/api_geo.c \
	http/api_crypt.c

sbin_PROGRAMS = shared

daemon_sources = $(common_INCLUDES) $(common_SOURCES)
daemon_flags = -g \
	-I$(top_srcdir)/src/share-lib \
	-I$(top_srcdir)/src/sexe
daemon_libs=
if DEBUG
daemon_flags += -DDEBUG -g
DBGLIB=
else
#DBGLIB=$(top_builddir)/src/share-log/libshlogd.a
DBGLIB=
daemon_flags += \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops \
	-Wno-discarded-qualifiers
endif
daemon_flags += -I$(srcdir)/dev/usb
daemon_libs += \
	$(USBLIB) $(DBGLIB)

if USE_USB
daemon_libs += $(USBLIB)
if HAVE_LIBUSB
daemon_flags += -Ilibusb-1.0
endif
endif

if LINUX
daemon_libs += -lrt
endif

shared_SOURCES = $(daemon_sources)
shared_LDADD = $(daemon_libs)
shared_CFLAGS = $(daemon_flags)

txtest_SOURCES = \
	$(srcdir)/sharedaemon_store.c \
	$(srcdir)/sharedaemon_store.h \
	$(srcdir)/dev/dev_clock.c \
	$(srcdir)/dev/clock_local.c \
	$(srcdir)/test/CuTest.c \
	$(srcdir)/test/CuTest.h \
	$(srcdir)/test/txtest_init.c \
	$(srcdir)/test/txtest_proc.c \
	$(srcdir)/test/txtest.h \
	$(srcdir)/test/txtest.c \
	bits/bits.c \
	bits/app.c \
	bits/transaction.c \
	bits/event.c \
	bits/signature.c \
	bits/metric.c \
	bits/run.c \
	bits/vm.c \
	bits/license.c \
	bits/trust.c \
	bits/account.c \
	bits/identity.c \
	bits/session.c \
	bits/ward.c \
	bits/clock.c \
	bits/eval.c \
	bits/reference.c \
	bits/context.c \
	bits/asset.c \
	bits/file.c \
	bits/subscribe.c \
	bits/init.c \
	bits/ledger.c \
	bits/wallet.c

txtest_LDADD = $(daemon_libs)
txtest_CFLAGS = $(daemon_flags)

#$(srcdir)/test/txtest.c: test/txtest_init.c test/txtest_proc.c test/txtest.h
#	$(top_srcdir)/scripts/make-tests.sh $(srcdir)/test/txtest_*.c > $(srcdir)/test/txtest.c


if HAVE_INITD_DIR
EXTRA_DIST = $(top_builddir)/scripts/initd/shared

install-exec-hook:
	rm -f $(INITD_DIR)/init.d/shared  
	rm -f $(INITD_DIR)/rc3.d/S85shared  
	rm -f $(INITD_DIR)/rc3.d/K85shared  
	rm -f $(INITD_DIR)/rc4.d/S85shared  
	rm -f $(INITD_DIR)/rc4.d/K85shared  
	rm -f $(INITD_DIR)/rc5.d/S85shared  
	rm -f $(INITD_DIR)/rc5.d/K85shared  
	cp $(top_builddir)/scripts/initd/shared $(INITD_DIR)/init.d/
	cd $(INITD_DIR)/rc3.d && $(LN_S) ../init.d/shared S85shared
	cd $(INITD_DIR)/rc3.d && $(LN_S) ../init.d/shared K85shared
	cd $(INITD_DIR)/rc4.d && $(LN_S) ../init.d/shared S85shared
	cd $(INITD_DIR)/rc4.d && $(LN_S) ../init.d/shared K85shared
	cd $(INITD_DIR)/rc5.d && $(LN_S) ../init.d/shared S85shared
	cd $(INITD_DIR)/rc5.d && $(LN_S) ../init.d/shared K85shared

uninstall-hook:
	rm -f $(INITD_DIR)/init.d/shared  
	rm -f $(INITD_DIR)/rc3.d/S85shared  
	rm -f $(INITD_DIR)/rc3.d/K85shared  
	rm -f $(INITD_DIR)/rc4.d/S85shared  
	rm -f $(INITD_DIR)/rc4.d/K85shared  
	rm -f $(INITD_DIR)/rc5.d/S85shared  
	rm -f $(INITD_DIR)/rc5.d/K85shared  
endif



