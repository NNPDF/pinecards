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
AM_CPPFLAGS = -I$(top_srcdir)
AM_LDFLAGS = $(ROOTLDFLAGS) $(ROOTLIBS) $(FCLIBS) $(HOPPETLIBS) $(FRTLLIB)

lib_LTLIBRARIES = libAPPLgrid.la libfAPPLgrid.la 
libAPPLgrid_la_SOURCES = \
	Cache.h \
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
	appl_igrid.h \
	appl_pdf.cxx \
	appl_timer.cxx \
	axis.h \
	basic_pdf.cxx \
	combination.cxx \
	dis_pdf.cxx \
	dis_pdf.h \
	fappl_grid.h \
	fastnlo.cxx \
	generic_pdf.cxx \
	hoppet_init.cxx	\
	hoppet_init.h \
	integral.cxx \
	jetrad_pdf.cxx \
	jetrad_pdf.h \
	lumi_pdf.cxx \
	mcfmQQ_pdf.cxx \
	mcfmQQ_pdf.h \
	mcfmw_pdf.cxx \
	mcfmw_pdf.h \
	mcfmwc_pdf.cxx \
	mcfmwc_pdf.h \
	mcfmwjet_pdf.cxx \
	mcfmwjet_pdf.h \
	mcfmz_pdf.cxx \
	mcfmz_pdf.h \
	mcfmzjet_pdf.cxx \
	mcfmzjet_pdf.h \
	nlojet_pdf.cxx \
	nlojet_pdf.h \
	nlojetpp_pdf.cxx \
	nlojetpp_pdf.h \
	sparse.h \
	tsparse1d.h \
	tsparse2d.h \
	tsparse3d.h \
	tsparse_base.h \
	vrapz_pdf.h

libfAPPLgrid_la_SOURCES = fappl_grid.cxx fappl_fitter.cxx

library_includedir=$(includedir)/appl_grid

library_include_HEADERS = \
	$(top_srcdir)/appl_grid/appl_grid.h \
	$(top_srcdir)/appl_grid/appl_igrid.h \
	$(top_srcdir)/appl_grid/appl_pdf.h \
	$(top_srcdir)/appl_grid/appl_timer.h \
	$(top_srcdir)/appl_grid/axis.h \
	$(top_srcdir)/appl_grid/basic_pdf.h \
	$(top_srcdir)/appl_grid/Cache.h \
	$(top_srcdir)/appl_grid/combination.h \
	$(top_srcdir)/appl_grid/correction.h \
	$(top_srcdir)/appl_grid/Directory.h \
	$(top_srcdir)/appl_grid/dis_pdf.h \
	$(top_srcdir)/appl_grid/fappl_grid.h \
	$(top_srcdir)/appl_grid/fastnlo.h \
	$(top_srcdir)/appl_grid/generic_pdf.h \
	$(top_srcdir)/appl_grid/hoppet_init.h \
	$(top_srcdir)/appl_grid/integral.h \
	$(top_srcdir)/appl_grid/jetrad_pdf.h \
	$(top_srcdir)/appl_grid/lumi_pdf.h \
	$(top_srcdir)/appl_grid/mcfmQQ_pdf.h \
	$(top_srcdir)/appl_grid/mcfmwc_pdf.h \
	$(top_srcdir)/appl_grid/mcfmwjet_pdf.h \
	$(top_srcdir)/appl_grid/mcfmw_pdf.h \
	$(top_srcdir)/appl_grid/mcfmzjet_pdf.h \
	$(top_srcdir)/appl_grid/mcfmz_pdf.h \
	$(top_srcdir)/appl_grid/nlojet_pdf.h \
	$(top_srcdir)/appl_grid/nlojetpp_pdf.h \
	$(top_srcdir)/appl_grid/Sparse3d.h \
	$(top_srcdir)/appl_grid/sparse.h \
	$(top_srcdir)/appl_grid/SparseMatrix3d.h \
	$(top_srcdir)/appl_grid/TFileString.h \
	$(top_srcdir)/appl_grid/TFileVector.h \
	$(top_srcdir)/appl_grid/tsparse1d.h \
	$(top_srcdir)/appl_grid/tsparse2d.h \
	$(top_srcdir)/appl_grid/tsparse3d.h \
	$(top_srcdir)/appl_grid/tsparse_base.h \
	$(top_srcdir)/appl_grid/vrapz_pdf.h

bin_PROGRAMS = applgrid-combine merge_bins pdf_info
applgrid_combine_SOURCES = combine.cxx
applgrid_combine_LDADD = libAPPLgrid.la
applgrid_combine_LDFLAGS = $(ROOTLDFLAGS) $(ROOTLIBS) $(HOPPETLIBS) $(FRTLLIB) $(FRTLIB)

merge_bins_SOURCES = merge_bins.cpp
merge_bins_LDADD = libAPPLgrid.la

pdf_info_SOURCES = pdf_info.cpp
pdf_info_LDADD = libAPPLgrid.la

TFileStringDict.cxx: TFileString.h TFileString.cxx
	$(AM_V_GEN)$(CINT) -f $@ -c $<

TFileVectorDict.cxx: TFileVector.h TFileVector.cxx
	$(AM_V_GEN)$(CINT) -f $@ -c $<

MOSTLYCLEANFILES = \
	TFileStringDict.cxx \
	TFileStringDict_rdict.pcm \
	TFileVectorDict.cxx \
	TFileVectorDict_rdict.pcm
