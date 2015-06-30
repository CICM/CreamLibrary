AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

pkglib_LTLIBRARIES = Cream.la

Cream_la_CXXFLAGS = @PD_CPPFLAGS@ -O3 -fPIC 
Cream_la_LDFLAGS = -module -avoid-version -shared -shrext .@EXTENSION@  @PD_LDFLAGS@ -fPIC
Cream_la_LIBADD = Deprecated/libDeprecated.la Sources/libSources.la
Cream_la_LIBADD += ThirdParty/CicmWrapper/Sources/libCicmWrapper.la

Cream_la_SOURCES = c.library.hpp \
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

$(abs_builddir)/.libs/Cream.@EXTENSION@: Cream.la

$(top_builddir)/Cream.@EXTENSION@: $(abs_builddir)/.libs/Cream.@EXTENSION@ 
		rm -f $@ && cd $(top_builddir) && test -e $< && $(LN_S) $< Cream.@EXTENSION@ || true

.PHONY: clean-conveniencelink help

clean-conveniencelink:
	test -L $(top_builddir)/Cream.@EXTENSION@ && rm -f $(top_builddir)/Cream.@EXTENSION@ || true


all-local:: $(top_builddir)/Cream.@EXTENSION@ help

clean-local:: clean-conveniencelink
	
README.txt:
	cp README.md README.txt
