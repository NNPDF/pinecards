AC_INIT(APPLgrid, 1.4.70, sutt@cern.ch,applgrid,http://applgrid.hepforge.org)
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
AC_PROG_CC
AC_PROG_CXX 
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
    ROOT_CXXFLAGS=$(root-config --cflags)
    ROOT_LDFLAGS=$(root-config --ldflags)
    ROOT_LDFLAGS+=" " 
    ROOT_LDFLAGS+=$(root-config --libs)
    AC_DEFINE_UNQUOTED(HAVE_CERNROOT,1,[Define 1 if root is installed]) 
fi

AC_SUBST(ROOT_LDFLAGS)
AC_SUBST(ROOT_CXXFLAGS)




# lhapdf isn't technically needed 
# AH_TEMPLATE([HAVE_LHAPDF],[Define 1 if lhapdf is installed])
# AC_PATH_PROG(LHAPDFPATH,lhapdf-config,no)

HOPPET_LDFLAGS=
AH_TEMPLATE([HAVE_HOPPET],[Define 1 if the hoppet library is installed])
AC_PATH_PROG(HOPPETPATH,hoppet-config,no)
if test "$HOPPETPATH" = "no"; then
    AC_MSG_WARN([********************************************************************])
    AC_MSG_WARN([])
    AC_MSG_WARN([   HOPPET was not found - you will not be able to arbirarily])
    AC_MSG_WARN([   change the factorisation scale in the convolution])
    AC_MSG_WARN([])
    AC_MSG_WARN([********************************************************************])
else 
    HOPPET_LDFLAGS=$(hoppet-config --libs)
    AC_DEFINE_UNQUOTED(HAVE_HOPPET,1,[Define 1 if hoppet is installed]) 
fi

AC_SUBST(HOPPET_LDFLAGS)
AM_CONDITIONAL(USE_HOPPET, [test "$HOPPETPATH" != "no"] )




LHAPDF_MAJOR_VERSION=0
LHAPDF_CXXFLAGS=
AH_TEMPLATE([HAVE_LHAPDF],[Define 1 if the LHAPDF library is installed])
AC_PATH_PROG(LHAPDFPATH,lhapdf-config,no)
if test "$LHAPDFPATH" != "no"; then

    # get LHAPDF major version - do nothing is version 6 not installed	
    LHAPDF_MAJOR_VERSION=$(lhapdf-config --version | sed 's/\..*//')

    if test $LHAPDF_MAJOR_VERSION -gt 5; then

       # find include path
       LHAPDF_CXXFLAGS=$(lhapdf-config --libs)

       AC_DEFINE_UNQUOTED(HAVE_LHAPDF,1,[Define 1 if LHAPDF is installed]) 
    
       LHAPDF_VERSION=$(lhapdf-config --version)

       AH_TEMPLATE([LHAPDF_VERSION],[LHAPDF version])
       AC_DEFINE_UNQUOTED(LHAPDF_VERSION,"$LHAPDF_VERSION",[LHAPDF version]) 

    fi
fi

AC_SUBST(LHAPDF_CXXFLAGS)
AM_CONDITIONAL(USE_LHAPDF, [test $LHAPDF_MAJOR_VERSION -gt 5 ] )


AC_CONFIG_FILES([bin/Makefile bin/applgrid-config])

AC_OUTPUT(Makefile src/Makefile)
