#
#

MAINTAINERCLEANFILES  = Makefile.in

EXTRA_DIST		       = welcome_service.cfg \
				 service_simple.vcproj \
				 common.cfg

sbin_PROGRAMS		= welcome_service

AM_CXXFLAGS               = -I$(top_srcdir)/src/common/include

welcome_service_LDADD	=	../../common/src/libwwcommon.la

welcome_service_SOURCES	=	CFESData.cpp		\
				CFESData.h		\
				CWelcomeService.cpp	\
				CWelcomeService.h	\
				fes_callbacks.cpp	\
				ls_callbacks.cpp

install-data-hook:
	mkdir -p ${DESTDIR}${pkgsysconfdir}
	test -f ${DESTDIR}${[kgsysconfdir}/welcome_service.cfg || cp welcome_service.cfg ${DESTDIR}${pkgsysconfdir} 

# End of Makefile.am

