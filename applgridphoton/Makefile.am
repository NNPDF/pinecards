AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

docdir = $(datadir)/doc/$(PACKAGE)
dist_doc_DATA = README INSTALL COPYING

sharedir = $(datadir)/$(PACKAGE)

dist_share_DATA = \
	$(top_srcdir)/share/basic.config \
	$(top_srcdir)/share/mcfmwm.config \
	$(top_srcdir)/share/mcfmwp.config \
	$(top_srcdir)/share/photonLO.config \
	$(top_srcdir)/share/photonNLO.config

SUBDIRS = src bin
