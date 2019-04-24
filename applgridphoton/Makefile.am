AUTOMAKE_OPTIONS = foreign 
ACLOCAL_AMFLAGS = -I m4
ACLOCAL  = aclocal
AUTOMAKE = automake

docdir   = ${datadir}/doc/${PACKAGE}
dist_doc_DATA   = README INSTALL COPYING

sharedir = ${datadir}/${PACKAGE}

clean-local:
	rm -rf autom4te.cache
dist_share_DATA = \
	$(top_srcdir)/share/basic.config \
	$(top_srcdir)/share/mcfmwm.config \
	$(top_srcdir)/share/mcfmwp.config \
	$(top_srcdir)/share/photonLO.config \
	$(top_srcdir)/share/photonNLO.config

SUBDIRS = src bin

## Clean out SVN files and root dictionaries
dist-hook:
	rm -rf `find $(distdir) -name ".svn"` `find $(distdir) -name \*Dict\*`
