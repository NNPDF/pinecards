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


ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --nonew --libs)
ROOTGLIBS  = $(shell root-config --nonew --glibs)
ROOTARCH   = $(shell root-config --ldflags)


if USE_HOPPET 
  HOPPETDIR    = $(shell hoppet-config --prefix)
  HOPPETCFLAGS = $(shell hoppet-config --cxxflags)
  HOPPETLIBS   = $(shell hoppet-config --libs)
else
  HOPPETDIR    = 
  HOPPETCFLAGS = 
  HOPPETLIBS   = 
endif


if USE_LHAPDF
  # in preparation for including the lhapdf version 6 interface
  LHAPDFCFLAGS = $(shell lhapdf-config --cxxflags)
else
  LHAPDFCFLAGS = 
endif


#FRTLIB   = $(shell gfortran -print-file-name=libgfortran.a)
FRTLLIB  = -lgfortran 

lib_LTLIBRARIES= libAPPLgrid.la libfAPPLgrid.la 
libAPPLgrid_la_SOURCES = \
	appl_grid.cxx		appl_igrid.cxx       fastnlo.cxx \
	appl_timer.cxx          appl_pdf.cxx         \
	nlojet_pdf.cxx		nlojetpp_pdf.cxx     \
	mcfmw_pdf.cxx		mcfmwjet_pdf.cxx \
	 mcfmwc_pdf.cxx       \
	mcfmz_pdf.cxx mcfmzjet_pdf.cxx  \
	 mcfmQQ_pdf.cxx       \
	jetrad_pdf.cxx		dis_pdf.cxx          \
	generic_pdf.cxx         basic_pdf.cxx        \
	combination.cxx         lumi_pdf.cxx         \
	SparseMatrix3d.cxx	hoppet_init.cxx	     \
	TFileString.cxx		TFileStringDict.cxx  \
	TFileVector.cxx		TFileVectorDict.cxx  \
	integral.cxx \
	appl_igrid.h \
	axis.h \
	Cache.h \
	dis_pdf.h \
	fappl_grid.h \
	hoppet_init.h \
	jetrad_pdf.h \
	mcfmQQ_pdf.h \
	mcfmwc_pdf.h \
	mcfmwjet_pdf.h \
	mcfmw_pdf.h \
	mcfmzjet_pdf.h \
	mcfmz_pdf.h \
	nlojet_pdf.h \
	nlojetpp_pdf.h \
	Sparse3d.h \
	sparse.h \
	SparseMatrix3d.h \
	TFileString.h \
	TFileVector.h \
	tsparse1d.h \
	tsparse2d.h \
	tsparse3d.h \
	tsparse_base.h \
	vrapz_pdf.h

libfAPPLgrid_la_SOURCES = fappl_grid.cxx fappl_fitter.cxx

library_includedir=$(includedir)/appl_grid

library_include_HEADERS= \
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
	$(top_srcdir)/appl_grid/TFileStringDict.h \
	$(top_srcdir)/appl_grid/TFileString.h \
	$(top_srcdir)/appl_grid/TFileVectorDict.h \
	$(top_srcdir)/appl_grid/TFileVector.h \
	$(top_srcdir)/appl_grid/tsparse1d.h \
	$(top_srcdir)/appl_grid/tsparse2d.h \
	$(top_srcdir)/appl_grid/tsparse3d.h \
	$(top_srcdir)/appl_grid/tsparse_base.h \
	$(top_srcdir)/appl_grid/vrapz_pdf.h

# GSLLIBS=$(shell gsl-config --libs)
# WDIR = $(shell pwd)

ROOTCINT=rootcint
AM_CXXFLAGS = $(ROOTARCH) -O2 -DDATADIR=\"@datadir@/$(PACKAGE)\" -Wall -Wextra -fPIC  -I. -I.. -I$(srcdir) $(LHAPDFCXXFLAGS) $(ROOTCFLAGS)  $(HOPPETCFLAGS) 

AM_LDFLAGS = $(ROOTARCH) -O2  $(ROOTLIBS) $(FCLIBS) $(HOPPETLIBS) $(FRTLLIB)


AM_FCFLAGS = -c
AM_FCLIBS  = -lg2c
AM_SOFLAGS = -shared
CINT       = rootcint

bin_PROGRAMS = applgrid-combine
applgrid_combine_SOURCES = combine.cxx
applgrid_combine_LDADD   = libAPPLgrid.la
applgrid_combine_LDFLAGS = $(ROOTARCH) $(ROOTLIBS) $(HOPPETLIBS) $(FRTLLIB) $(FRTLIB) 


clean-local:
	rm -rf *.o *.lo *Dict*
.cxx.o : 
	$(CXX) $(AM_CXXFLAGS) -c $<
.c.o : 
	$(CC) $(AM_CFLAGS) -c $<
%Dict.cxx : %.h %.cxx
	$(CINT) -f $@ -c $< 

#../appl_grid/$*LinkDef.h
