ACLOCAL_AMFLAGS= -I m4

if HAVE_OPENSSL_HMAC

noinst_LIBRARIES = libshare_oauth.a
libshare_oauth_a_SOURCES = \
	shauth_client.c shauth_server.c shauth.h \
	oauth.c config.h hash.c xmalloc.c xmalloc.h oauth_http.c oauth.h

endif

#if HAVE_CURL
#liboauth_a_CFLAGS=-DHAVE_CURL
#endif
