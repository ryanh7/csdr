ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libcsdr.la
libcsdr_la_SOURCES = fft_fftw.c fft_rpi.c libcsdr.c libcsdr_gpl.c fastddc.c ima_adpcm.c fastddc.h fft_fftw.h fft_rpi.h ima_adpcm.h libcsdr_gpl.h libcsdr.h predefined.h
libcsdr_la_LDFLAGS = -version-info 1:0:0
libcsdr_la_CFLAGS = $(FFTW3_CFLAGS)
libcsdr_la_LIBADD = $(FFTW3_LIBS)

bin_PROGRAMS = csdr
csdr_SOURCES = csdr.c
csdr_LDADD = libcsdr.la
