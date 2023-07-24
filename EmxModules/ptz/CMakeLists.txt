cmake_minimum_required(VERSION 3.10)

set(TARGET_LIB Ptz)
aux_source_directory(lib SRCS_LIB)
aux_source_directory(lib/motorGpio SRCS_LIB)
add_library(${TARGET_LIB} SHARED ${SRCS_LIB})
strip(${TARGET_LIB} lib${TARGET_LIB}.so)
target_include_directories(${TARGET_LIB} PRIVATE include)
target_include_directories(${TARGET_LIB} PRIVATE lib)
target_include_directories(${TARGET_LIB} PRIVATE lib/motorGpio)

#install
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})
