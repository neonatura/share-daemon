#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([shared], [4.0])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/share-daemon])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST([with_kernel], [`uname -r`])
AC_SUBST([with_kerneldir], [/lib/modules/$with_kernel])
AC_PATH_KERNEL_SOURCE
AC_SUBST([KERNEL_SRC], [$with_kerneldir/build])
AC_SUBST([KERNEL_MOD], [$with_kerneldir/extra])
AM_CONDITIONAL([KERNEL_2_6_32], [test `uname -r|cut -d- -f1` = "2.6.32"])
AM_CONDITIONAL([KMOD_DIR], [test -f /lib/modules/`uname -r`/build])

LT_INIT

version_patch=0
AC_SUBST(version_patch)
version_info=4:0:0
AC_SUBST(version_info)

# docs
AC_CONFIG_FILES([bin/README.txt], [chmod -wx bin/README.txt])

# Architecture
AC_MSG_CHECKING([for supported operating system])
case "$host_os" in
  *linux*)
  opsys=linux
  AC_DEFINE([LINUX], 1, [Define for the Linux operating system.])
  AC_DEFINE([POLL_NFDS_TYPE],[nfds_t],[type of second poll() argument])
  AC_CHECK_HEADERS([poll.h])
  AC_CHECK_TYPE([nfds_t],
    [AC_DEFINE([POLL_NFDS_TYPE],[nfds_t],[type of second poll() argument])],
    [AC_DEFINE([POLL_NFDS_TYPE],[unsigned int],[type of second poll() argument])],
    [#include <poll.h>])
  ;;
  windows)
  opsys=win
  AC_DEFINE([WINDOWS], 1, [Define for the Windows operating system.])
  AC_DEFINE([POLL_NFDS_TYPE],[unsigned int],[type of second poll() argument])
  ;;
  mingw*)
  opsys=win
AC_DEFINE([WINDOWS], 1, [Define for the Windows operating system.])
  ;;
  msys*)
  opsys=win
AC_DEFINE([WINDOWS], 1, [Define for the Windows operating system.])
  ;;
  sunos4*)
  opsys=sunos4
AC_DEFINE([SUNOS4], 1, [Define for the SunOS 4.x operating system.])
  ;;
  solaris2* | sysv[[45]]* | irix[[56]]*)
  opsys=svr4
  AC_DEFINE([SVR4], 1, [Define for the System V release 4 operating
      system or a derivative like Solaris 2.x or Irix 5.x.])
  case "$host_os" in
  sysv4.2uw*)
AC_DEFINE(UNIXWARE, 2, [Define for UnixWare systems.])
  ;;
  sysv5*)
AC_DEFINE(UNIXWARE, 7, [Define for UnixWare systems.])
  ;;
  esac
  ;;
  freebsd*)
  opsys=freebsd
AC_DEFINE([FREEBSD], 1, [Define for the FreeBSD operating system.])
  ;;
  *)
  AC_MSG_RESULT([NO!])
AC_MSG_ERROR([operating system $host_os is not supported.])
  ;;
  esac
  AC_MSG_RESULT($opsys)
AC_MSG_CHECKING([for supported architecture])
  case "$host_cpu" in
  bfin)
  arch=bfin
AC_DEFINE([BFIN], 1, [Define for the Blackfin architecture.])
  ;;
  i[[3456]]86|pentium)
  arch=i386
AC_DEFINE([I386], 1, [Define for the i386 architecture.])
  ;;
  mingw32)
  arch=i386
AC_DEFINE([I386], 1, [Define for the i386 architecture.])
  ;;
  ia64)
  arch=ia64
AC_DEFINE([IA64], 1, [Define for the IA64 architecture.])
  ;;
  m68k)
  arch=m68k
AC_DEFINE([M68K], 1, [Define for the m68k architecture.])
  ;;
  sparc64*)
  arch=sparc64
AC_DEFINE([SPARC64], 1, [Define for the SPARC64 architecture.])
  ;;
  sparc*)
  arch=sparc
AC_DEFINE([SPARC], 1, [Define for the SPARC architecture.])
  ;;
  mips*)
  arch=mips
AC_DEFINE([MIPS], 1, [Define for the MIPS architecture.])
  ;;
  alpha*)
  arch=alpha
AC_DEFINE([ALPHA], 1, [Define for the Alpha architecture.])
  ;;
  powerpc*)
  arch=powerpc
AC_DEFINE([POWERPC], 1, [Define for the PowerPC architecture.])
  ;;
  arm*)
  arch=arm
AC_DEFINE([ARM], 1, [Define for the ARM architecture.])
  ;;
  avr32*)
  arch=avr32
AC_DEFINE([AVR32], 1, [Define for the AVR32 architecture.])
  ;;
  s390)
  arch=s390
AC_DEFINE([S390], 1, [Define for the S390 architecture.])
  ;;
  s390x)
  arch=s390x
AC_DEFINE([S390X], 1, [Define for the S390x architecture.])
  ;;
  hppa*|parisc*)
  arch=hppa
AC_DEFINE([HPPA], 1, [Define for the HPPA architecture.])
  ;;
  sh64*)
  arch=sh64
AC_DEFINE([SH64], 1, [Define for the SH64 architecture.])
  ;;
  sh*)
  arch=sh
AC_DEFINE([SH], 1, [Define for the SH architecture.])
  ;;
  x86?64*)
  arch=x86_64
AC_DEFINE([X86_64], 1, [Define for the AMD x86-64 architecture.])
  ;;
  mingw64)
  arch=x86_64
AC_DEFINE([X86_64], 1, [Define for the AMD x86-64 architecture.])
  ;;
  cris|crisv10)
  arch=crisv10
AC_DEFINE([CRISV10], 1, [Define for the CRISv10 architecture.])
  ;;
  crisv32)
  arch=crisv32
AC_DEFINE([CRISV32], 1, [Define for the CRISv32 architecture.])
  ;;
  tile*)
  arch=tile
AC_DEFINE([TILE], 1, [Define for the Tile architecture])
  ;;
  *)
  AC_MSG_RESULT([NO!])
AC_MSG_ERROR([architecture $host_cpu is not supported.])
  ;;
  esac
AC_MSG_RESULT($arch)
AC_SUBST(opsys)
AC_SUBST(arch)
AM_CONDITIONAL([LINUX], [test x$opsys = xlinux])
AM_CONDITIONAL([WINDOWS], [test x$opsys = xwin])
AM_CONDITIONAL([I386], [test x$arch = xi386])
AM_CONDITIONAL([X86_64], [test x$arch = xx86_64])
AM_CONDITIONAL([SUNOS4], [test x$opsys = xsunos4])
AM_CONDITIONAL([SVR4], [test x$opsys = xsvr4])
AM_CONDITIONAL([FREEBSD], [test x$opsys = xfreebsd])


# Checks for programs.
AC_PROG_CC
gl_EARLY

AC_PROG_CXX
AM_PROG_AS
AC_EXEEXT
AC_PROG_YACC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_LIBTOOL
AC_PROG_YACC
AM_PROG_CC_C_O
AM_PROG_GCJ

AC_SYS_LARGEFILE
AC_ISC_POSIX
AC_C_INLINE

CPPFLAGS="$CPPFLAGS -I/usr/include/php/Zend -I/usr/include/php/TSRM -I/usr/include/php/main -I/usr/include/php"
AC_SUBST(CPPFLAGS)

# Checks for header files.
AC_CHECK_HEADERS(fcntl.h linux/fd.h memory.h net/errno.h float.h \
  sgtty.h string.h iconv.h fnmatch.h \
  sys/param.h sys/device.h sys/gentape.h \
  sys/inet.h sys/io/trioctl.h sys/ioctl.h \
  sys/mtio.h sys/time.h sys/tprintf.h sys/tape.h time.h sys/stat.h \
  paths.h unistd.h locale.h arpa/inet.h limits.h \
  netdb.h netinet/in.h stddef.h stdint.h stdio.h stdlib.h sys/socket.h sys/resource.h unistd.h, wctype.h wchar.h)
AC_CHECK_HEADERS(math.h)
AC_CHECK_HEADERS(signal.h)
AC_CHECK_HEADERS(pthread.h)
AC_CHECK_HEADERS(sys/resource.h)
AC_CHECK_HEADERS(sys/select.h)
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(wtype.h)
AC_CHECK_HEADERS(wchar.h)
AC_CHECK_HEADERS(wcchar.h)
AC_CHECK_HEADERS(utime.h)
AC_CHECK_HEADERS(pwd.h)
AC_CHECK_HEADERS(paths.h)
AC_CHECK_HEADERS(net/if.h)
AC_CHECK_HEADERS(net/if_arp.h)
AC_CHECK_HEADERS(obstack.h)
AC_CHECK_HEADERS(sys/statvfs.h)
AC_CHECK_HEADERS(shadow.h)

# check ssl/hmac
AC_CHECK_HEADER([openssl/hmac.h], 
  AM_CONDITIONAL([HAVE_OPENSSL_HMAC], true)
  AC_DEFINE(HAVE_OPENSSL_HMAC,1,[Have openssl hmac capability.]),
  AM_CONDITIONAL([HAVE_OPENSSL_HMAC], false))

AC_HEADER_SYS_WAIT
AC_HEADER_DIRENT
AC_HEADER_MAJOR
AC_HEADER_STAT
AC_HEADER_STDC

AC_CHECK_LIB([m], [fabs])
AC_CHECK_LIB([pthread], [pthread_self])
AC_CHECK_LIB([rt], [clock_gettime])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_CHECK_TYPES([ptrdiff_t stdout fpos_t])
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)

AC_CHECK_FUNC(fnmatch)

gt_TYPE_SSIZE_T
gl_INIT

AC_CHECK_DECLS([getgrgid],,, [#include <grp.h>])
AC_CHECK_DECLS([getpwuid],,, [#include <pwd.h>])
AC_CHECK_DECLS([time],,, [#include <time.h>])
AC_CHECK_DECLS([isblank],,,[#include <ctype.h>])

AC_REPLACE_FUNCS(waitpid)

AC_CHECK_TYPES([struct stat])
AC_CHECK_TYPES([struct stat64])
AC_CHECK_TYPES([struct sockaddr_in6])
AC_CHECK_TYPES([struct in6_addr])
AC_CHECK_TYPES([struct ifreq])
AC_CHECK_TYPES([struct passwd])

# mmap() support
AC_FUNC_MMAP
if test x$ac_cv_func_mmap_fixed_mapped = xyes; then
  AC_CHECK_FUNCS(munmap msync)
fi

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_MEMCMP
AC_FUNC_STRFTIME
AC_FUNC_ALLOCA
AC_CHECK_FUNCS([getcwd gethostbyname gettimeofday memmove memset mkdir _mkdir socket strchr strdup strerror strrchr atoi strtol strtoll inet_addr ftell stat fstat getrusage fopen fclose fread fwrite pthread_mutex_lock fchmod fchown fsync lstat mkfifo readlink symlink kill ftruncate memchr regcomp select setenv setlocale strtoumax socket ioctl fcntl connect bind accept listen pthread_mutex_unlock calloc gettid pthread_self pthread_mutex_init pthread_mutex_destroy strtod killpg sysconf bsearch isalpha getservbyname getpwnam getpwuid getlogin_r getspnam getrlimit glob])
# for refclock
AC_CHECK_FUNCS([nice plock pututline pututxline readlink rtprio])
AC_CHECK_FUNCS(htonll ntohll)
AC_CHECK_FUNCS(strsignal)
AC_CHECK_FUNCS([modechange])
AC_CHECK_FUNCS([xstrndup])
AC_CHECK_FUNCS([basename])
AC_CHECK_FUNCS([dirname])
AC_CHECK_FUNCS([faccessat])
AC_CHECK_FUNCS([_spawnv])
AC_CHECK_FUNCS([getpwent])
AC_CHECK_FUNCS([getpwent_r])
AC_CHECK_FUNCS([opendir])
AC_CHECK_FUNCS([closedir])

AC_CHECK_LIB([share], [get_libshare_version], , 
  AC_MSG_ERROR("libshare (runtime library) is required")
)

AC_CHECK_MEMBERS([struct stat.st_rdev])

AC_CHECK_DECL([_PATH_BSHELL],,AC_DEFINE_UNQUOTED([_PATH_BSHELL], "/bin/sh", [Define to system shell path]),[
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
])
AC_CHECK_DECL([_PATH_TTY],,AC_DEFINE_UNQUOTED([_PATH_TTY], "/dev/tty", [Define to tty device node path]),[
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
])
AC_CHECK_DECL([_PATH_DEVNULL],,AC_DEFINE_UNQUOTED([_PATH_DEVNULL], "/dev/null", [Define to devnull device node path]),[
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
])

AC_ARG_ENABLE([shfsyncd],
  AS_HELP_STRING([--disable-shfsyncd], [Disable the file-sync daemon.]))
AM_CONDITIONAL(HAVE_FSYNC_DAEMON, [test "x$enable_shfsyncd" != "xno"])


#AC_ARG_ENABLE([leitch],
#  AS_HELP_STRING([--disable-leitch], [Disable support for leitch time devices.]))
#AM_CONDITIONAL(CLOCK_LEITCH_DEVICE, [test "x$enable_leitch" != "xno"])
#if test "x$enable_leitch" != "xno" ; then
#  AC_DEFINE_UNQUOTED([CLOCK_LEITCH_DEVICE], 1, [Define to enable Leitch time device.])
#fi

AC_ARG_ENABLE([shared_api],
  AS_HELP_STRING([--disable-shared-api], [Disable the shared web API.]))
AM_CONDITIONAL(SHARED_API_SERVICE, [test "x$enable_shared_api" != "xno"])
if test "x$enable_shared_api" != "xno" ; then
  AC_DEFINE_UNQUOTED([SHARED_API_SERVICE], 1, [Define to enable shared HTTP API service.])
fi






# usb device config
usb=false

AC_ARG_ENABLE([refclock],
  AS_HELP_STRING([--enable-refclock], [Enable time synchronization support.]))
AM_CONDITIONAL(REFCLOCK, [test "x$enable_refclock" = "xyes"])
if test "x$enable_refclock" = "xyes" ; then
  AC_DEFINE_UNQUOTED([REFCLOCK], 1, [Define shared time synchronization.])
  AC_DEFINE([USE_USB], 1, [Define for enabling use of USB hardware.])
  usb=true
fi

AC_ARG_ENABLE([magtek],
  AS_HELP_STRING([--enable-magtek], [Enable MagTek Mini Swipe Reader Device]))
AM_CONDITIONAL(USE_MAGTEK_DEVICE, [test "x$enable_magtek" = "xyes"])
if test "x$enable_magtek" = "xyes" ; then
  AC_DEFINE_UNQUOTED([USE_MAGTEK_DEVICE], 1,[Define to enable MagTek Mini Swipe Reader Device])
  AC_DEFINE([USE_USB], 1, [Define for enabling use of USB hardware.])
  usb=true
fi

AC_ARG_ENABLE([ztex],
  AS_HELP_STRING([--enable-ztex], [Enable ZTex FPGA 1.15y Device]))
AM_CONDITIONAL(USE_ZTEX_DEVICE, [test "x$enable_ztex" = "xyes"])
if test "x$enable_ztex" = "xyes" ; then
  AC_DEFINE_UNQUOTED([USE_ZTEX_DEVICE], 1, [Define to enable ZTex FPGA 1.15y Device])
  AC_DEFINE([USE_USB], 1, [Define for enabling use of USB hardware.])
  usb=true
fi

#AC_ARG_ENABLE(usb,
#AS_HELP_STRING([--enable-usb],
#               [enable usb hardware; default: no]),
#[case "${enableval}" in
#             yes) usb=true ;;
#             no)  usb=false ;;
#             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-usb]) ;;
#esac], [usb=false])
HAVE_LIBUSB=0
AM_CONDITIONAL(USE_USB, test x"$usb" = x"true")
if test "x$usb" = "xtrue" ; then
  AC_DEFINE([USE_USB], 1, [Define for enabling use of USB hardware.])

  AC_CHECK_LIB([usb-1.0], [libusb_close], HAVE_LIBUSB=1, 
    AC_MSG_ERROR([The USB-1.0 library must be installed. (centos) yum install libusb1 libusb1-devel]))
fi
AM_CONDITIONAL(HAVE_LIBUSB, test x"$HAVE_LIBUSB" = x"1") 
if test x"$HAVE_LIBUSB" = x"1"; then
  AC_DEFINE_UNQUOTED([HAVE_LIBUSB], 1, [Define if the USB-1.0 library is installed.])
fi

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac], [debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")
if test "x$debug" = "xtrue" ; then
  AC_DEFINE_UNQUOTED([DEBUG], 1, [Define for printing debugging information.])
fi

AC_CHECK_PROGS([CP], [cp])

AC_CHECK_PROGS([HTML2TEXT], [html2text])
AM_CONDITIONAL([HAVE_HTML2TEXT], [test -n "$HTML2TEXT"])
AC_SUBST([HTML2TEXT], [test -n "$HTML2TEXT"])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([LIBUNISTRING_COMPILE_UNIWIDTH_WIDTH], [false])
AM_CONDITIONAL([HAVE_DOXYGEN],
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/shared/doxygen.conf])])

# help2man
AC_CHECK_PROG(HELP2MAN, [help2man], [help2man], [true])
if test -z "$HELP2MAN";
 then AC_MSG_WARN([help2man not found - continuing without manual page generation.])
fi
AM_CONDITIONAL(HAVE_HELP2MAN, [test x"$HELP2MAN" = x"help2man"])

#AC_PATH_PROG([PHP], php)
#AM_CONDITIONAL(HAVE_PHP, [test x"$PHP" = x"php"])
AC_CHECK_PROGS(PHPCONFIG, php-config php5-config php-config5 php4-config php-config4)
AC_MSG_CHECKING(for PHP extension-dir)
PHPLIBDIR="`$PHPCONFIG --extension-dir 2>/dev/null`"
if test ! -z "$PHPLIBDIR"; then
  AC_MSG_RESULT($PHPLIBDIR)
else
  AC_MSG_RESULT(not found)
fi
AC_SUBST([PHPLIBDIR])

AC_CHECK_PROGS([SYSTEMCTL], [systemctl])
AM_CONDITIONAL([HAVE_SYSTEMCTL], [test -n "$SYSTEMCTL"])

HAVE_LIBGMP=0
AC_ARG_ENABLE([ecdsa],
  AS_HELP_STRING([--disable-ecdsa], [Disable the ECDSA algorythm.]))
if test "x$enable_ecdsa" != "xno" ; then
  AC_DEFINE([HAVE_LIBGMP],1,[Enable GMP library.])
  HAVE_LIBGMP=1
fi
AM_CONDITIONAL(HAVE_LIBGMP, test $HAVE_LIBGMP) 

# configure generic readline library 
HAVE_LIBREADLINE=0
AC_CHECK_LIB([readline], [readline], 
  HAVE_LIBREADLINE=1
  AC_DEFINE([HAVE_LIBREADLINE],1,[Enable readline library.]))
AM_CONDITIONAL(HAVE_LIBREADLINE, test $HAVE_LIBREADLINE) 

AC_DEFINE_UNQUOTED([XD3_POSIX], 1, [XDELTA3])
AC_DEFINE_UNQUOTED([USE_POSIX_FILES], 1, [XDELTA3])
AC_DEFINE_UNQUOTED([GENERIC_ENCODE_TABLES], 0, [XDELTA3])
AC_DEFINE_UNQUOTED([HAVE_HIDDEN], 1, [LIBZ])
AC_DEFINE_UNQUOTED([Z_PREFIX], 1, [LIBZ])
AC_DEFINE_UNQUOTED([LUA_COMPAT_ALL], 1, [SEXE])
AC_DEFINE_UNQUOTED([LUA_ANSI], 1, [SEXE])
AC_DEFINE_UNQUOTED([POLARSSL_X509_USE_C], 1, [X509])
AC_DEFINE_UNQUOTED([POLARSSL_RSA_C], 1, [X509])
AC_DEFINE_UNQUOTED([POLARSSL_BIGNUM_C], 1, [X509])
AC_DEFINE_UNQUOTED([POLARSSL_PLATFORM_C], 1, [X509])



AC_OUTPUT(Makefile src/share-oauth-lib/Makefile src/share-daemon/Makefile src/share-fsync-daemon/Makefile doc/shared/Makefile bin/Makefile) 

