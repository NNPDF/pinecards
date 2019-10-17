##########################################################################
#
#   File:         Makefile.am
#
#                 Copyright (C) M.Sutton (sutt@cern.ch)
#
#   Description:  generate the Makefile for the appl_grid project
#
#
#   Created:    M.Sutton (sutt@hep.ucl.ac.uk) Fri  5 Nov 2010 00:05:29 GMT
#
#   $Id: makefile 20 2008-05-05 01:55:42Z sutt $
#
#########################################################################


ROOTCFLAGS  = @ROOTCFLAGS@
ROOTLIBS    = @ROOTLIBS@
ROOTGLIBS   = @ROOTGLIBS@
ROOTLDFLAGS = @ROOTLDFLAGS@

HOPPETDIR    = @HOPPETDIR@
HOPPETCFLAGS = @HOPPETCFLAGS@
HOPPETLIBS   = @HOPPETLIBS@

LHAPDFCFLAGS = @LHAPDFCFLAGS@

FRTLLIB = -lgfortran

CINT = @ROOTCINT@

AM_CXXFLAGS = $(ROOTLDFLAGS) -DDATADIR=\"@datadir@/$(PACKAGE)\" -fPIC $(LHAPDFCXXFLAGS) $(ROOTCFLAGS) $(HOPPETCFLAGS)
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/appl_grid
AM_LDFLAGS = $(ROOTLDFLAGS) $(ROOTLIBS) $(FCLIBS) $(HOPPETLIBS) $(FRTLLIB)

lib_LTLIBRARIES = libAPPLgrid.la
libAPPLgrid_la_SOURCES = \
	Cache.h \
	Directory.h \
	Sparse3d.h \
	SparseMatrix3d.cxx \
	SparseMatrix3d.h \
	TFileString.cxx \
	TFileString.h \
	TFileStringDict.cxx \
	TFileVector.cxx \
	TFileVector.h \
	TFileVectorDict.cxx \
	appl_grid.cxx \
	appl_igrid.cxx \
	appl_pdf.cxx \
	appl_timer.cxx \
	appl_timer.h \
	axis.h \
	combination.cxx \
	hoppet_init.cxx	\
	hoppet_init.h \
	lumi_pdf.cxx \
	sparse.h \
	tsparse1d.h \
	tsparse2d.h \
	tsparse3d.h \
	tsparse_base.h

library_includedir=$(includedir)/appl_grid

library_include_HEADERS = \
	$(top_srcdir)/appl_grid/appl_grid.h \
	$(top_srcdir)/appl_grid/appl_igrid.h \
	$(top_srcdir)/appl_grid/appl_pdf.h \
	$(top_srcdir)/appl_grid/combination.h \
	$(top_srcdir)/appl_grid/lumi_pdf.h

bin_PROGRAMS = applgrid-combine
applgrid_combine_SOURCES = combine.cxx
applgrid_combine_LDADD = libAPPLgrid.la
applgrid_combine_LDFLAGS = $(ROOTLDFLAGS) $(ROOTLIBS) $(HOPPETLIBS) $(FRTLLIB) $(FRTLIB)

TFileStringDict.cxx: TFileString.h TFileString.cxx
	$(AM_V_GEN)$(CINT) -f $@ -c $<

TFileVectorDict.cxx: TFileVector.h TFileVector.cxx
	$(AM_V_GEN)$(CINT) -f $@ -c $<

MOSTLYCLEANFILES = \
	TFileStringDict.cxx \
	TFileStringDict_rdict.pcm \
	TFileVectorDict.cxx \
	TFileVectorDict_rdict.pcm
