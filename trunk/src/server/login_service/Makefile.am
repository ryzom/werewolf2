#
#

MAINTAINERCLEANFILES  = Makefile.in

EXTRA_DIST		       = login_service.cfg \
				 login_service.vcproj \
				 common.cfg

sbin_PROGRAMS		= login_service

AM_CXXFLAGS               = -I$(top_srcdir)/src/common/include

login_service_LDADD	=	../../common/src/libwwcommon.la

login_service_SOURCES	=	callbacks.cpp		\
				callbacks.h		\
				CLoginService.cpp	\
				CLoginService.h

install-data-hook:
	mkdir -p ${DESTDIR}${pkgsysconfdir}
	test -f ${DESTDIR}${[kgsysconfdir}/login_service.cfg || cp login_service.cfg ${DESTDIR}${pkgsysconfdir} 

# End of Makefile.am

