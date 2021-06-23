cmake_minimum_required (VERSION 3.0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project (digiham VERSION 0.5.0)
add_definitions(-DVERSION="${PROJECT_VERSION}-dev")

enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)

include(GNUInstallDirs)

find_package(Threads REQUIRED)

include(CheckLibraryExists)
check_library_exists(m floorf "" CSDR_HAS_LIBM)

include(FindPkgConfig)
pkg_check_modules(FFTW3 fftw3f)

if(NOT DEFINED CSDR_GPL)
    set(CSDR_GPL true)
endif()

if(NOT DEFINED CSDR_IMA_ADPCM)
    set(CSDR_IMA_ADPCM true)
endif()

SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(CMAKE_C_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O3")
SET(CMAKE_C_FLAGS_DEBUG "-g -O3")

add_subdirectory(src)
