# ====================================================================
# Configuration script for Werewolf
# ====================================================================

# ====================================================================
# Process this file with autoconf to produce a configure script.
# ====================================================================

# If you want to change the version, must must change AC_INIT
# *and* AC_SUBST(LIBTOOL_VERSION)

AC_PREREQ(2.57)
AC_INIT([werewolf],[0.4.0],[mraykowski@gmail.com])


AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE


AM_CONFIG_HEADER(config.h)

AC_SUBST(LIBTOOL_VERSION, [0:4:0])



# The following hack should ensure that configure doesn't add optimizing
# or debugging flags to CFLAGS or CXXFLAGS
CFLAGS="$CFLAGS "
CXXFLAGS="$CXXFLAGS "

# Template needed to generate the config.h.in
AH_TEMPLATE([WW_CONFIG],[Configuration files directory path])
AH_TEMPLATE([WW_LOGS],[Log files directory path])
AH_TEMPLATE([WW_STATE],[Local state files directory path])
AH_TEMPLATE([HAVE_MYSQL],[Define to 1 if mysql was found])


# ====================================================================
# Checks for programs.
# ====================================================================

AC_PROG_MAKE_SET

AC_PROG_CC
AC_PROG_CXX

AM_SANITY_CHECK

AC_PROG_INSTALL

AC_PROG_LIBTOOL
AM_PROG_LIBTOOL

AM_PATH_CCACHE


AM_PATH_MYSQL(3.23.40,,AC_MSG_ERROR([MySQL 3.23.40 or higher must be installed]))

AC_DEFINE(HAVE_MYSQL)

CFLAGS="$CFLAGS $MYSQL_CFLAGS"
CXXFLAGS="$CXXFLAGS $MYSQL_CFLAGS"

LIBS="$LIBS -lrt $MYSQL_LIBS"

# ====================================================================
# MySQL++
AM_PATH_MYSQLPP

CFLAGS="$CFLAGS $MYSQLPP_CFLAGS"
CXXFLAGS="$CXXFLAGS $MYSQLPP_CFLAGS"

LIBS="$LIBS -lrt $MYSQLPP_LIBS"

# ====================================================================
# LibXML

# Use C compiler as a default for the libxml tests.
AC_LANG([C])

AM_PATH_XML2([2.0.0], [], [AC_MSG_ERROR([libxml2 must be installed])])

CFLAGS="$CFLAGS $XML_CFLAGS $XML_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $XML_CFLAGS $XML_CPPFLAGS"

LIBS="$LIBS $XML_LIBS"


# ====================================================================
# Configure Settings
# ====================================================================

AC_LANG([C++])


# ====================================================================
# Debug/optimized compilation mode
# ====================================================================

AM_NEL_DEBUG

# Set the gcc specific warning level
if test "X$CC" = "Xgcc";
then
    if test "$with_debug" != "no"
    then
	#
	# When debugging variables are declared for the sole purpose of
	# inspecting their content with a debugger. They are not used
	# in the code itself and this is legitimate, hence the -Wno-unused
	#
        CXXFLAGS="$CXXFLAGS -Wall -Wno-multichar -Wno-unused"
    else
        CXXFLAGS="$CXXFLAGS -Wall -Wno-multichar"
    fi
fi

# ====================================================================
# Checks for typedefs, structures, and compiler characteristics.
# ====================================================================

# Add the define _REENTRANT for a correct use of the threads
if test "X$CC" = "Xgcc";
then
    CXXFLAGS="$CXXFLAGS -D_REENTRANT"
fi


# ====================================================================
# Checks for header files.
# ====================================================================

AC_HEADER_STDC([])
AC_CHECK_HEADERS(fcntl.h unistd.h)


# ====================================================================
# Checks compilation accelerator
# ====================================================================

AM_PATH_CCACHE

# ====================================================================
# Checks for libraries.
# ====================================================================

AC_CHECK_LIB(crypt, crypt)

# ====================================================================
# STLPort

AM_PATH_STLPORT


# ====================================================================
# NeL

#         misc   net    3d    pacs  sound  ai    georges
AM_PATH_NEL("yes", "yes", "yes", "yes", "no",  "no", "yes")

# ====================================================================
# Checks for library functions.
# ====================================================================

# ====================================================================
# nelns configuration and logs file location
# ====================================================================

test "x$prefix" = xNONE && prefix=$ac_default_prefix

AC_ARG_WITH(sysconfdir,
    [  --with-sysconfdir[=DIR]
                          use DIR instead of the default sysconfdir/ww],
    [pkgsysconfdir=$withval],
    [pkgsysconfdir='${sysconfdir}/ww'])
AC_SUBST(pkgsysconfdir)

eval eval WW_CONFIG=\"$pkgsysconfdir/\"
AC_DEFINE_UNQUOTED(WW_CONFIG, "$WW_CONFIG")

AC_ARG_WITH(localstatedir,
    [  --with-localstatedir[=DIR]
                          use DIR instead of the default localstatedir/game/nelns],
    [pkglocalstatedir=$withval],
    [pkglocalstatedir=${localstatedir}/game/ww])
AC_SUBST(pkglocalstatedir)

eval eval WW_STATE=\"$pkglocalstatedir/\"
AC_DEFINE_UNQUOTED(WW_STATE, "$WW_STATE")

AC_ARG_WITH(logdir,
    [  --with-logdir[=DIR]
                          use DIR instead of the default localstatedir/log/nelns],
    [logdir=$withval],
    [logdir='${localstatedir}/log/ww'])
AC_SUBST(logdir)

eval eval WW_LOGS=\"$logdir/\"
AC_DEFINE_UNQUOTED(WW_LOGS, "$WW_LOGS")

# ====================================================================
# Output files to generate.
# ====================================================================

AC_CONFIG_FILES([	Makefile				\
			src/Makefile				\
			src/common/Makefile			\
			src/common/src/Makefile			\
			src/server/Makefile			\
			src/server/login_service/Makefile	\
			src/server/service_simple/Makefile	\
			src/server/service_frontend/Makefile	
])
AC_OUTPUT


# End of configure.ac
