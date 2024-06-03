cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(inner)

set(TARGET_LIB Key)
aux_source_directory(lib SRCS_LIB)
add_library(${TARGET_LIB} SHARED ${SRCS_LIB})
strip(${TARGET_LIB} lib${TARGET_LIB}.so)
target_include_directories(${TARGET_LIB} PRIVATE ../bsp/include)
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
