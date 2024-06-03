cmake_minimum_required(VERSION 3.10)

set(TARGET_LIB Bsp)

#include_directories(include)
aux_source_directory(lib LIB_SRCS)
add_library(${TARGET_LIB} SHARED ${LIB_SRCS})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})

