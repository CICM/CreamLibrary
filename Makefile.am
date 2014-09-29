AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

pkglib_LTLIBRARIES = cream.la

cream_la_CXXFLAGS = @PD_CPPFLAGS@ -s -O2 -fPIC -fpermissive 
cream_la_LDFLAGS = -module -avoid-version -shared -shrext .@EXTENSION@  @PD_LDFLAGS@ -fPIC
cream_la_LIBADD = Coffee/libCoffee.la Caramel/libCaramel.la Chocolate/libChocolate.la
cream_la_LIBADD += ThirdParty/PureData/Sources/libCicmWrapper.la

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

SUBDIRS+= ThirdParty/PureData
SUBDIRS+= Package/CreamLibrary

libdir: all
	$(INSTALL) -p -m 755 -d $(DISTBINDIR)
	$(INSTALL_DATA) $(PACKAGE_NAME)-meta.pd  $(DISTBINDIR)
	$(INSTALL_DATA) $(PACKAGE_NAME).$(EXTENSION)  $(DISTBINDIR)
	$(INSTALL_DATA) $(dist_cream_DATA) $(DISTBINDIR)
	test -z "$(strip $(EXTRA_DIST))" || \
		$(INSTALL_DATA) $(EXTRA_DIST)    $(DISTBINDIR)
#	tar --exclude-vcs -czpf $(DISTBINDIR).tar.gz $(DISTBINDIR)

$(abs_builddir)/.libs/cream.@EXTENSION@: cream.la

$(top_builddir)/cream.@EXTENSION@: $(abs_builddir)/.libs/cream.@EXTENSION@ 
		rm -f $@ && cd $(top_builddir) && test -e $< && $(LN_S) $< cream.@EXTENSION@ || true

.PHONY: clean-conveniencelink help

clean-conveniencelink:
	test -L $(top_builddir)/cream.@EXTENSION@ && rm -f $(top_builddir)/cream.@EXTENSION@ || true


all-local:: $(top_builddir)/cream.@EXTENSION@ help

clean-local:: clean-conveniencelink

.o: .cc
	$(CXX) $(ALL_CFLAGS) -o "$*.o" -c "$*.cc"
	
README.txt:
	cp README.md README.txt
