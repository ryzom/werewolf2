#
#

MAINTAINERCLEANFILES  = Makefile.in

EXTRA_DIST		       = service_frontend.cfg \
				 service_frontend.vcproj \
				 common.cfg

AM_CXXFLAGS               = -I$(top_srcdir)/src/common/include

sbin_PROGRAMS		= service_frontend

service_frontend_LDADD	=	../../common/src/libwwcommon.la

service_frontend_SOURCES	=	CActor.cpp		\
					CActorEventHandler.cpp	\
					CActorEventHandler.h	\
					CActor.h		\
					callbacks.cpp		\
					callbacks.h		\
					CCharacterManager.cpp	\
					CCharacterManager.h	\
					CFrontendService.cpp	\
					CFrontendService.h	\
					CServerSimulation.cpp	\
					CServerSimulation.h	\
					CUserController.cpp	\
					CUserController.h	\
					CPerformerDRView.cpp	\
					CPerformerDRView.h	\
					stdpch.h		

# I don't think these are used anymore.
#					CPacsTask.cpp		
#					CPacsTask.h		
#					CTimeTask.cpp		
#					CTimeTask.h		

install-data-hook:
	mkdir -p ${DESTDIR}${pkgsysconfdir}
	test -f ${DESTDIR}${[kgsysconfdir}/service_frontend.cfg || cp service_frontend.cfg ${DESTDIR}${pkgsysconfdir} 

# End of Makefile.am
