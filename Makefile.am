ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libcsdr.la
libcsdr_la_SOURCES = fft_fftw.c fft_rpi.c libcsdr.c libcsdr_gpl.c fastddc.c ima_adpcm.c fastddc.h fft_fftw.h fft_rpi.h ima_adpcm.h libcsdr_gpl.h libcsdr.h predefined.h
libcsdr_la_includedir = $(includedir)
libcsdr_la_include_HEADERS = libcsdr.h
libcsdr_la_LDFLAGS = -release $(PACKAGE_VERSION)
libcsdr_la_CFLAGS = $(FFTW3_CFLAGS)
libcsdr_la_LIBADD = $(FFTW3_LIBS)

bin_PROGRAMS = csdr nmux
csdr_SOURCES = csdr.c benchmark.c
csdr_LDADD = libcsdr.la $(FFTW3_LIBS)
csdr_CFLAGS = -DCSDR_VERSION=\"$(PACKAGE_VERSION)\"

nmux_SOURCES = nmux.cpp tsmpool.h tsmpool.cpp
nmux_CXXFLAGS = $(PTHREAD_CFLAGS)
nmux_LDADD = $(PTHREAD_LIBS)