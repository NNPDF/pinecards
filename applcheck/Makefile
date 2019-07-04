#############################################
# Makefile to generate c++ programs
# make         => for build project
# make clean   => for clean project
#############################################

# Files
##################
EXECUTABLE=  applcheck

GSLINCLUDE= $(shell gsl-config --cflags)
GSLLIBS= $(shell gsl-config --libs)

# applgrid
APPLCXXFLAGS = $(shell applgrid-config --cxxflags)
APPLCLIBS    = $(shell applgrid-config --ldcflags)

# Compiler options
##################
CC=g++
CFLAGS=-Wall -O0 $(shell root-config --cflags) -ggdb
CFLAGSD=-Wall -O0 $(shell root-config --cflags) -ggdb
LDFLAGS=-c -Wall $(shell root-config --cflags) -ggdb
LDFLAGSD=-c -Wall -O0 -g $(shell root-config --cflags) -ggdb
LIBS= $(shell lhapdf-config --libs) $(shell root-config --glibs)  $(shell gsl-config --libs)  $(APPLCLIBS)
INCL= -I$(shell lhapdf-config --incdir) $(shell gsl-config --cflags) $(APPLCXXFLAGS)

# Data
##################
SRCDIR=
INCLUDEDIR=
FILENAME=$(addsuffix .cc, $(CCFILES))
FILENAMEH=$(addsuffix .h, $(HFILES))
SOURCES=$(addprefix $(SRCDIR), $(FILENAME))
INCLUDES=$(addprefix $(INCLUDEDIR), $(FILENAMEH))
OBJECTS=$(SOURCES:.cc=.o)
OBJECTSD=$(SOURCES:.cc=_d.o)

FILEMAIN=$(addsuffix .cc, $(EXECUTABLE))
SOURCEMAIN=$(addprefix $(SRCDIR), $(FILEMAIN))
MAINS=$(SOURCEMAIN:.cc=.o)
MAINSD=$(SOURCEMAIN:.cc=_d.o)

all: release

install: all

uninstall: clean

release: $(SOURCES) $(MAINS) $(EXECUTABLE)

debug: $(SOURCES) $(MAINSD) $(EXECUTABLE_DEBUG)

$(EXECUTABLE_DEBUG): $(OBJECTSD) $(INCLUDES) $(SRCDIR)/$<
	$(CC) $(CFLAGSD) $(OBJECTSD) $(SRCDIR)$@.o -I $(INCLUDEDIR) $(LIBS) $(INCL) \
	-o $@
	@echo "==> Build $@"

%_d.o: %.cc 
	$(CC) $(LDFLAGSD) -I $(INCLUDEDIR) $(INCL) $(LIBS) $< -o $@

$(EXECUTABLE): $(OBJECTS) $(INCLUDES) $(SRCDIR)/$<
	$(CC) $(CFLAGS) $(OBJECTS) $(SRCDIR)$@.o -I $(INCLUDEDIR) $(INCL) $(LIBS) \
	-o $@
	@echo "==> Build $@"

%.o: %.cc
	$(CC) $(LDFLAGS) -I $(INCLUDEDIR) $(LIBS) $(INCL) $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(EXECUTABLE_DEBUG)
	rm -rf $(SRCDIR)*.o $(SRCDIR)*.d $(SRCDIR)*~ *.eps *.root *.dat *.C *.rep* plot/*.eps
