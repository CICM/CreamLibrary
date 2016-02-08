AUTOMAKE_OPTIONS = foreign
noinst_LTLIBRARIES = libSources.la

libSources_la_CXXFLAGS = @PD_CPPFLAGS@ -O3 -fPIC
libSources_la_LDFLAGS = -module -avoid-version -shared -shrext .@EXTENSION@  @PD_LDFLAGS@ -fPIC
libSources_la_LIBADD = 

libSources_la_SOURCES = c.bang.cpp \
c.blackboard.cpp \
c.breakpoints.cpp \
c.colorpanel.cpp \
c.dsp_tilde.cpp \
c.gain_tilde.cpp \
c.incdec.cpp \
c.knob.cpp \
c.matrix.cpp \
c.matrix_tilde \
c.menu.cpp \
c.meter_tilde.cpp \
c.number.cpp \
c.number_tilde.cpp \
c.plane.cpp \
c.preset.cpp \
c.radio.cpp \
c.rslider.cpp \
c.scope_tilde.cpp \
c.slider.cpp \
c.tab.cpp \
c.toggle.cpp


