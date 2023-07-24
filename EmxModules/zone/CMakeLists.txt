cmake_minimum_required(VERSION 3.10)

set(TARGET_LIB Zone)

include_directories(include)
aux_source_directory(lib LIB_SRCS)
add_library(${TARGET_LIB} SHARED ${LIB_SRCS})
strip(${TARGET_LIB} lib${TARGET_LIB}.so)
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/zone)
