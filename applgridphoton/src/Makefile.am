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
	SparseMatrix3d.cxx \
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
	combination.cxx \
	histo1d.cpp \
	hoppet_init.cxx	\
	hoppet_init.h \
	lumi_pdf.cxx \
	sparse_matrix3d_root_interface.cpp \
	sparse_matrix3d_root_interface.hpp \
	tsparse1d.h \
	tsparse2d.h \
	tsparse3d.cxx \
	tsparse_base.cxx

library_includedir=$(includedir)/appl_grid

library_include_HEADERS = \
	$(top_srcdir)/appl_grid/SparseMatrix3d.h \
	$(top_srcdir)/appl_grid/appl_grid.h \
	$(top_srcdir)/appl_grid/appl_igrid.h \
	$(top_srcdir)/appl_grid/appl_pdf.h \
	$(top_srcdir)/appl_grid/axis.h \
	$(top_srcdir)/appl_grid/combination.h \
	$(top_srcdir)/appl_grid/histo1d.hpp \
	$(top_srcdir)/appl_grid/lumi_pdf.h \
	$(top_srcdir)/appl_grid/tsparse3d.h \
	$(top_srcdir)/appl_grid/tsparse_base.h

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
