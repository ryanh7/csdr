AC_INIT([csdr], 0.15.2)
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIRS([m4])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_ENABLE_SHARED
AC_DISABLE_STATIC
LT_INIT
AC_CONFIG_FILES(Makefile)
AC_CHECK_LIB([m],[floorf])
PKG_CHECK_MODULES([FFTW3], [fftw3f])
AX_PTHREAD
AX_GCC_FUNC_ATTRIBUTE([ifunc])
AC_DEFINE(USE_FFTW)
AC_DEFINE(USE_IMA_ADPCM)
AC_DEFINE(LIBCSDR_GPL)
AC_ARG_ENABLE([neon],
    AS_HELP_STRING([[[--enable-neon]]], [Enable ARM CPU NEON optimizations]),
    [case $enableval in
        no|off)
            enable_neon=no
            ;;
        *)
            enable_neon=yes
            ;;
    esac], [if grep "neon" /proc/cpuinfo > /dev/null ; then
        enable_neon=yes
    else
        enable_neon=no
    fi])
if test yes = $enable_neon; then
    AC_DEFINE([NEON_OPTS])
    PARAMS_SIMD="-mfloat-abi=hard -mfpu=neon -mvectorize-with-neon-quad"
fi
case `uname -m` in
    arm*|aarch64*)
        PARAMS_ARCH="-march=armv7-a -mtune=cortex-a8 -funsafe-math-optimizations -Wformat=0"
        CFLAGS="-g -O3"
        CXXFLAGS="-g -O3"
        ;;
    x86_64)
        AS_IF([test yes = AS_VAR_GET([ax_cv_have_func_attribute_ifunc])],
            [CFLAGS="-g -O3"
            CXXFLAGS="-g -O3"])
        ;;
esac
CFLAGS="${CFLAGS} ${PARAMS_ARCH} ${PARAMS_SIMD}"
CXXFLAGS="${CXXFLAGS} ${PARAMS_ARCH} ${PARAMS_SIMD}"
AC_OUTPUT