AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

pkglib_LTLIBRARIES = cream.la

cream_la_CXXFLAGS = @PD_CPPFLAGS@ -s -O2 -fPIC -fpermissive 
cream_la_LDFLAGS = -module -avoid-version -shared -shrext .@EXTENSION@  @PD_LDFLAGS@ -fPIC
cream_la_LIBADD = Coffee/libCoffee.la Caramel/libCaramel.la Chocolate/libChocolate.la
cream_la_LIBADD += ThirdParty/PureData/Sources/ebox/libebox.la \
 ThirdParty/PureData/Sources/eclass/libeclass.la \
 ThirdParty/PureData/Sources/ecommon/libecommon.la \
 ThirdParty/PureData/Sources/egraphics/libegraphics.la \
 ThirdParty/PureData/Sources/epopup/libepopup.la \
 ThirdParty/PureData/Sources/eobj/libeobj.la
#~cream_la_LIBADD += ThirdParty/PureData/Sources/ebox/.libs/libebox.a ThirdParty/PureData/Sources/eclass/.libs/libeclass.a ThirdParty/PureData/Sources/ecommon/.libs/libecommon.a ThirdParty/PureData/Sources/egraphics/.libs/libegraphics.a ThirdParty/PureData/Sources/eobj/.libs/libeobj.a ThirdParty/PureData/Sources/epopup/.libs/libepopup.a
#~cream_la_LIBADD += ThirdParty/PureData/Sources/libCicmWrapper.la
#~cream_la_LIBADD += ThirdParty/PureData/Sources/.libs/libCicmWrapper.a

cream_la_SOURCES = c.library.h \
 c.library.cpp

ALL_CFLAGS = -I"$(PD_INCLUDE)"

creamdir=$(pkglibdir)

dist_cream_DATA = \
	cream-meta.pd \
	LICENSE.txt \
	README.txt
	
EXTRA_DIST = autogen.sh

DISTDIR=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
ORIGDIR=pd-$(PACKAGE_NAME)_$(PACKAGE_VERSION)
OS=$(shell uname -s)
DISTBINDIR=$(DISTDIR)-$(OS)

SUBDIRS=Coffee Caramel Chocolate

SUBDIRS+= ThirdParty/PureData/Sources/ebox \
 ThirdParty/PureData/Sources/eclass \
 ThirdParty/PureData/Sources/ecommon \
 ThirdParty/PureData/Sources/egraphics \
 ThirdParty/PureData/Sources/eobj \
 ThirdParty/PureData/Sources/epopup

libdir: all
	$(INSTALL) -p -m 755 -d $(DISTBINDIR)
	$(INSTALL_DATA) $(PACKAGE_NAME)-meta.pd  $(DISTBINDIR)
	$(INSTALL_DATA) $(PACKAGE_NAME).$(EXTENSION)  $(DISTBINDIR)
	$(INSTALL_DATA) $(dist_cream_DATA) $(DISTBINDIR)
	test -z "$(strip $(EXTRA_DIST))" || \
		$(INSTALL_DATA) $(EXTRA_DIST)    $(DISTBINDIR)
#	tar --exclude-vcs -czpf $(DISTBINDIR).tar.gz $(DISTBINDIR)

$(abs_builddir)/.libs/cream.@EXTENSION@: cream.la

$(top_builddir)/c.library.@EXTENSION@: $(abs_builddir)/.libs/cream.@EXTENSION@ 
		rm -f $@ && cd $(top_builddir) && test -e $< && $(LN_S) $< c.library.@EXTENSION@ || true

.PHONY: clean-conveniencelink help

clean-conveniencelink:
	test -L $(top_builddir)/cream.@EXTENSION@ && rm -f $(top_builddir)/cream.@EXTENSION@ || true


all-local:: $(top_builddir)/c.library.@EXTENSION@ help

clean-local:: clean-conveniencelink

.o: .cc
	$(CXX) $(ALL_CFLAGS) -o "$*.o" -c "$*.cc"
	
README.txt:
	cp README.md README.txt
