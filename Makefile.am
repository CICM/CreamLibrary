AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

pkglib_LTLIBRARIES = Cream.la

Cream_la_CXXFLAGS = @PD_CPPFLAGS@ -O3 -fPIC 
Cream_la_LDFLAGS = -module -avoid-version -shared -shrext .@EXTENSION@  @PD_LDFLAGS@ -fPIC
Cream_la_LIBADD = Deprecated/libDeprecated.la Sources/libSources.la
Cream_la_LIBADD += ThirdParty/CicmWrapper/Sources/libCicmWrapper.la

Cream_la_SOURCES = c.library.h \
 c.library.cpp

ALL_CFLAGS = -I"$(PD_INCLUDE)"

Creamdir=$(pkglibdir)

dist_Cream_DATA = \
	Cream-meta.pd \
	LICENSE.txt \
	README.txt
	
EXTRA_DIST = autogen.sh

DISTDIR=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
ORIGDIR=pd-$(PACKAGE_NAME)_$(PACKAGE_VERSION)
OS=$(shell uname -s)
DISTBINDIR=$(DISTDIR)-$(OS)

SUBDIRS= Deprecated Sources
SUBDIRS+= ThirdParty/CicmWrapper
SUBDIRS+= Package/Cream

$(abs_builddir)/.libs/cream.@EXTENSION@: Cream.la

$(top_builddir)/cream.@EXTENSION@: $(abs_builddir)/.libs/cream.@EXTENSION@ 
		rm -f $@ && cd $(top_builddir) && test -e $< && $(LN_S) $< cream.@EXTENSION@ || true

.PHONY: clean-conveniencelink help

clean-conveniencelink:
	test -L $(top_builddir)/cream.@EXTENSION@ && rm -f $(top_builddir)/cream.@EXTENSION@ || true


all-local:: $(top_builddir)/cream.@EXTENSION@ help

clean-local:: clean-conveniencelink
	
README.txt:
	cp README.md README.txt
