


noinst_SCRIPTS = \
	.notice

noinst_PROGRAMS = shared

if HAVE_DOXYGEN
noinst_SCRIPTS += html man
endif


html: $(top_builddir)/doc/shared/html
	ln -sf $(top_builddir)/doc/shared/html .

man: $(top_builddir)/doc/shared/man
	ln -sf $(top_builddir)/doc/shared/man .


.notice:
	@echo > .notice
	@echo ""
	@echo "The share daemon has been built."
	@echo ""
	@echo "It is now safe to perform a 'make install'."
	@echo "Perform a 'make check' for additional tests afterwards."
	@echo ""
	@echo "Documentation is located in the bin/html/ directory."
	@echo ""




shared$(EXEEXT):
	ln -sf $(top_builddir)/src/share-daemon/shared$(EXEEXT) .
