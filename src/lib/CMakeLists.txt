add_library(csdr++ SHARED module.cpp ringbuffer.cpp writer.cpp agc.cpp fmdemod.cpp amdemod.cpp dcblock.cpp converter.cpp fft.cpp window.cpp logpower.cpp logaveragepower.cpp fftexchangesides.cpp realpart.cpp shift.cpp firdecimate.cpp fir.cpp benchmark.cpp reader.cpp fractionaldecimator.cpp adpcm.cpp limit.cpp power.cpp)
set_target_properties(csdr++ PROPERTIES VERSION ${PROJECT_VERSION})
file(GLOB LIBCSDR_HEADERS "${PROJECT_SOURCE_DIR}/include/*.hpp")
set_target_properties(csdr++ PROPERTIES PUBLIC_HEADER "${LIBCSDR_HEADERS}")
target_link_libraries(csdr++ ${FFTW3_LIBRARIES})
target_compile_definitions(csdr++ PRIVATE "-D_GNU_SOURCE")

if (HAS_IFUNC)
    target_compile_definitions(csdr++ PUBLIC "-DCSDR_FMV")
endif()

if (HAS_NEON)
    target_compile_definitions(csdr++ PUBLIC "-DNEON_OPTS")
endif()

install(TARGETS csdr++
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/csdr"
)