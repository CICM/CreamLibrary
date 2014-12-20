AUTOMAKE_OPTIONS = foreign
noinst_LTLIBRARIES = libCoffee.la

libCoffee_la_CXXFLAGS = @PD_CPPFLAGS@ -s -O2 -fPIC -fpermissive 
libCoffee_la_LDFLAGS = -module -avoid-version -shared -shrext .@EXTENSION@  @PD_LDFLAGS@ -fPIC
libCoffee_la_LIBADD = 

libCoffee_la_SOURCES = c.informer.cpp \
c.io.cpp \
c.loadmess.cpp \
c.mousestate.cpp \
c.pak.cpp \
c.patcherargs.cpp \
c.patcherinfos.cpp \
c.patchermess.cpp \
c.prepend.cpp


