##########################################################################
#
#   File:         Makefile.am
#
#                 Copyright (C) M.Sutton (sutt@cern.ch) 
#
#   Description:  generate the Makefile for the amcatnlo - applgrid bridge
#                 
#
#   Created:    M.Sutton (sutt@hep.ucl.ac.uk) Fri 10 July 2013 00:05:29 GMT
#   
#   $Id: Makefile.am 20 2013-07-10 01:55:42Z sutt $                
#                   
#########################################################################

lib_LIBRARIES = libamcfast.a 

libamcfast_a_SOURCES = amcfast_interface.cxx

library_includedir      = $(includedir)/amcfast
library_include_HEADERS = amcfast_interface.h amcatnlo_common.h

APPLCFLAGS  = $(shell applgrid-config --cxxflags)
APPLLIBS    = $(shell applgrid-config --ldcflags)

LHAPDFCFLAGS = $(shell lhapdf-config --cppflags)
LHAPDFLIBS   = $(shell lhapdf-config --ldflags)

bin_PROGRAMS            = amcfast-psgrid
amcfast_psgrid_SOURCES  = amcfast-psgrid.cxx
amcfast_psgrid_CXXFLAGS = -O2 -Wall -fPIC -DPDFPATH=\"$(LHAPDFPATH)\" -I. -I.. -I$(srcdir) $(APPLCFLAGS) $(LHAPDFCFLAGS)
amcfast_psgrid_LDFLAGS  = -L. -lamcfast $(APPLLIBS) $(LHAPDFLIBS)

AM_CXXFLAGS = -O2 -Wall -fPIC -I. -I.. -I$(srcdir) $(APPLCFLAGS) $(LHAPDFCFLAGS)
AM_CFLAGS   = -O2 -Wall -fPIC -I. -I.. -I$(srcdir) $(APPLCFLAGS) $(LHAPDFCFLAGS)
AM_CPPFLAGS = -O2 -Wall -fPIC -I. -I.. -I$(srcdir) $(APPLCFLAGS) $(LHAPDFCFLAGS)

AM_LDFLAGS = -O $(APPLLIBS)

AM_FCFLAGS = -c
AM_FCLIBS  = -lg2c
AM_SOFLAGS = -shared

clean-local:
	rm -rf *.o *.lo *Dict*

.cxx.o : 
	$(CXX) $(AM_CXXFLAGS) -c $<

.c.o :
	$(CC) $(AM_CFLAGS) -c $<

