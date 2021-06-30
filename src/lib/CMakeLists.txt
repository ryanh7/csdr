add_library(csdr++ module.cpp ringbuffer.cpp writer.cpp agc.cpp fmdemod.cpp amdemod.cpp dcblock.cpp converter.cpp fft.cpp window.cpp logpower.cpp logaveragepower.cpp fftexchangesides.cpp realpart.cpp)
set_target_properties(csdr PROPERTIES VERSION ${PROJECT_VERSION})
file(GLOB LIBCSDR_HEADERS "${PROJECT_SOURCE_DIR}/include/*.hpp")
set_target_properties(csdr PROPERTIES PUBLIC_HEADER "${LIBCSDR_HEADERS}")
target_link_libraries(csdr++ ${FFTW3_LIBRARIES})

install(TARGETS csdr++
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/csdr"
)