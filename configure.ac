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
AS_IF([test yes = AS_VAR_GET([ax_cv_have_func_attribute_ifunc])],
    [CFLAGS="-g -O3"
    CXXFLAGS="-g -O3"])
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
            case `uname -m` in
                arm*|aarch64*)
                    enable_neon=yes
                    AC_DEFINE([NEON_OPTS])
                    CFLAGS="${CFLAGS} -mfloat-abi=hard -mfpu=neon -mvectorize-with-neon-quad"
                    ;;
                *)
                    enable_neon=no
            esac
        ;;
    esac], [case `uname -m` in
        arm*|aarch64*)
            if [ grep "neon" /proc/cpuinfo ] ; then
                enable_neon=yes
                AC_DEFINE([NEON_OPTS])
                CFLAGS="${CFLAGS} -mfloat-abi=hard -mfpu=neon -mvectorize-with-neon-quad"
            else
                enable_neon=no
            fi
            ;;
        *)
            enable_neon=no
    esac])
case `uname -m` in
    arm*|aarch64*)
        CFLAGS="${CFLAGS} -march=armv7-a -mtune=cortex-a8 -funsafe-math-optimizations -Wformat=0"
    ;;
esac
AC_OUTPUT