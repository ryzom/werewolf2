#
# $Id: Makefile.am,v 1.2 2007/01/04 16:47:55 mattr Exp $
#

MAINTAINERCLEANFILES = Makefile.in configure config.guess missing \
                       config.sub ltconfig aclocal.m4 config.h.in \
                       install-sh mkinstalldirs depcomp $(DISTCLEANFILES)

DISTCLEANFILES       = stamp-h.in config.cache config.log config.status 

EXTRA_DIST	     = acconfig.h services.dsw bootstrap doc

SUBDIRS              = frontend_service          \
                       common_lib                \
                       testclient 

dist-hook:
	find $(distdir) -name CVS -print | xargs rm -fr

install-data-hook:
	mkdir -p ${DESTDIR}${pkglocalstatedir}
	mkdir -p ${DESTDIR}${logdir}
	mkdir -p ${DESTDIR}${pkgsysconfdir}
#
# Do not override existing config files
#
	for service in admin admin_executor login naming welcome ; \
	do \
		test -f ${DESTDIR}${pkgsysconfdir}/$${service}_service.cfg || cp ${srcdir}/$${service}_service/$${service}_service.cfg ${DESTDIR}${pkgsysconfdir} ; \
	done
	test -f ${DESTDIR}${pkgsysconfdir}/common.cfg || cp ${srcdir}/admin_service/common.cfg ${DESTDIR}${pkgsysconfdir}
 

uninstall-local:
	for service in admin admin_executor login naming welcome ; \
	do \
		rm -f ${DESTDIR}${pkgsysconfdir}/$${service}_service.cfg ; \
	done
	rm -f ${DESTDIR}${pkgsysconfdir}/common.cfg

# End of Makefile.am
