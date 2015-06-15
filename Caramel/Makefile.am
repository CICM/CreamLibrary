AUTOMAKE_OPTIONS = foreign
noinst_LTLIBRARIES = libCaramel.la

libCaramel_la_CXXFLAGS = @PD_CPPFLAGS@ -O3 -fPIC
libCaramel_la_LDFLAGS = -module -avoid-version -shared -shrext .@EXTENSION@  @PD_LDFLAGS@ -fPIC
libCaramel_la_LIBADD = 

libCaramel_la_SOURCES = AudioFFT.cpp \
AudioFFT.h \
c.convolve_tilde.cpp \
c.fir_tilde.cpp \
c.freeverb_tilde.cpp \
c.matrix_tilde.cpp \
FFTConvolver.cpp \
FFTConvolver.h \
fftw3.h \
Utilities.cpp \
Utilities.h

