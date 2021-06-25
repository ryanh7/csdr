add_executable(csdr++-bin csdr++.cpp commands.cpp)
set_target_properties(csdr++-bin PROPERTIES OUTPUT_NAME csdr++)
target_link_libraries(csdr++-bin csdr++)
install(TARGETS csdr++-bin DESTINATION ${CMAKE_INSTALL_BINDIR})
