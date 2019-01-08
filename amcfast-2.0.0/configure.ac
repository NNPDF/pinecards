AC_INIT(aMCfast, 2.0.0, valerio.bertone@cern.ch, amcfast, http://amcfast.hepforge.org)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(amconfig.h:amconfig.in)
#AC_PROG_FC
#AC_CHECK_PROGS(FC, [gfortran g77 f77 fort77 f90], no)
#AC_PROG_FC
#AC_LANG_PUSH(Fortran)
#AC_FC_SRCEXT(f90)
#AC_FC_LIBRARY_LDFLAGS
#AC_FC_WRAPPERS
#AC_FC_FREEFORM
#AC_FC_DUMMY_MAIN
#AC_LANG_POP(Fortran)
AC_PROG_CC([clang gcc])
AC_PROG_CXX([clang++ g++])
AM_PROG_LIBTOOL
AC_HEADER_STDC
AC_FUNC_UTIME_NULL
AC_CONFIG_MACRO_DIR([m4])
## Set Fortran compiler behaviour
#if test "x$FCFLAGS" == "x"; then 
#  FCFLAGS="-O2"
#fi
## Try to respect users' Fortran compiler variables
#if test "x$FC" == "x"; then 
#  if test "x$F77" == "x"; then 
#FC="$GFORTRAN"
#FC=gfortran
#  else
#    FC="$F77"
#  fi
#fi

AH_TEMPLATE([HAVE_CERNROOT],[Define 1 if the cern root package is installed])
AC_PATH_PROG(ROOTPATH,root,no)
if test "$ROOTPATH" = "no"; then
    AC_MSG_WARN([****************************************************])
    AC_MSG_WARN([])
    AC_MSG_WARN([root was not found - needed for applgrid storage])
    AC_MSG_WARN([])
    AC_MSG_WARN([****************************************************])
else
    AC_DEFINE_UNQUOTED(HAVE_CERNROOT,1,[Define 1 if root is installed]) 
fi

# lhapdf isn't technically needed 
AH_TEMPLATE([HAVE_LHAPDF],[Define 1 if lhapdf is installed])
AC_PATH_PROG(LHAPDFPATH,lhapdf-config,no)
if test "$LHAPDFPATH" = "no"; then
    AC_MSG_WARN([****************************************************])
    AC_MSG_WARN([])
    AC_MSG_WARN([lhapdf was not found])
    AC_MSG_WARN([])
    AC_MSG_WARN([****************************************************])
else
    AC_DEFINE_UNQUOTED(HAVE_LHAPDF,1,[Define 1 if lhapdf is installed]) 
fi


AH_TEMPLATE([HAVE_APPL],[Define 1 if the applgrid library is installed])
AC_PATH_PROG(APPLPATH,applgrid-config,no)
if test "$APPLPATH" = "no"; then
    AC_MSG_WARN([********************************************************************])
    AC_MSG_WARN([])
    AC_MSG_WARN([   APPLgrid was not found - you will not be able to do anything! ])
    AC_MSG_WARN([])
    AC_MSG_WARN([********************************************************************])
else 
    AC_DEFINE_UNQUOTED(HAVE_APPL,1,[Define 1 if applgrid is installed]) 
fi


AC_CONFIG_FILES([bin/Makefile bin/amcfast-config bin/amcfast-shower])


AC_OUTPUT(Makefile src/Makefile)
