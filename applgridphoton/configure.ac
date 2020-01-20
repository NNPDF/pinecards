AC_INIT(APPLgrid, 1.4.70, sutt@cern.ch,applgrid,http://applgrid.hepforge.org)
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE
LT_INIT
AM_CONFIG_HEADER(amconfig.h:amconfig.in)
AC_PROG_CXX 
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])
AC_CONFIG_MACRO_DIR([m4])

AC_PATH_PROG(ROOTCONFIG,root-config,no)
AC_PATH_PROG(ROOTCINT,rootcint,no)
AC_SUBST(ROOTCINT)

AS_IF([test x"${ROOTCONFIG}" = x"no" || test x"${ROOTCINT}" = x"no"],
    AC_MSG_ERROR([root-config was not found - needed for applgrid storage]), [

    AC_DEFINE_UNQUOTED(HAVE_CERNROOT,1,[Define 1 if root is installed]) 

    ROOTCFLAGS=`root-config --cflags`
    AC_SUBST(ROOTCFLAGS)
    ROOTLIBS=`root-config --nonew --libs`
    AC_SUBST(ROOTLIBS)
    ROOTGLIBS=`root-config --nonew --glibs`
    AC_SUBST(ROOTGLIBS)
    ROOTLDFLAGS="`root-config --ldflags` $ROOTLIBS"
    AC_SUBST(ROOTLDFLAGS)
])

AC_PATH_PROG(HOPPETCONFIG,hoppet-config,no)

AS_IF([test x"${HOPPETCONFIG}" = x"no"],
    AC_MSG_WARN([hoppet-config was not found - you will not be able to arbirarily change the factorisation scale in the convolution]), [

    AC_DEFINE_UNQUOTED(HAVE_HOPPET,1,[Define 1 if hoppet is installed]) 

    HOPPETDIR=`hoppet-config --prefix`
    AC_SUBST(HOPPETDIR)
    HOPPETCFLAGS=`hoppet-config --cxxflags`
    AC_SUBST(HOPPETCFLAGS)
    HOPPETLIBS=`hoppet-config --libs`
    AC_SUBST(HOPPETLIBS)
])

AC_PATH_PROG(LHAPDFCONFIG,lhapdf-config,no)

AS_IF([test x"${LHAPDFCONFIG}" = x"no"], [], [
    AS_IF([test `lhapdf-config --version | sed 's/\..*//'` -gt 5], [
        AC_DEFINE_UNQUOTED(LHAPDF_VERSION,`lhapdf-config --version`,[LHAPDF version]) 
        AC_DEFINE_UNQUOTED(HAVE_LHAPDF,1,[Define 1 if LHAPDF is installed]) 
        LHAPDFCFLAGS=`lhapdf-config --cxxflags`
        LHAPDFLIBS=`lhapdf-config --libs`
        AC_SUBST(LHAPDFCPPFLAGS)
        AC_SUBST(LHAPDFLIBS)
    ])
])

AC_CONFIG_FILES([bin/Makefile bin/applgrid-config])

AC_OUTPUT(Makefile src/Makefile)
