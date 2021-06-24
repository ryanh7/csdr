add_library(csdr++ module.cpp ringbuffer.cpp writer.cpp agc.cpp)
set_target_properties(csdr PROPERTIES VERSION ${PROJECT_VERSION})
file(GLOB LIBCSDR_HEADERS "${PROJECT_SOURCE_DIR}/include/*.hpp")
set_target_properties(csdr PROPERTIES PUBLIC_HEADER "${LIBCSDR_HEADERS}")

install(TARGETS csdr++
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/csdr"
)