cmake_minimum_required(VERSION 3.10)

set(TARGET_LIB EmxProtoV1)

include_directories(.)
include_directories(include)

add_library(${TARGET_LIB} SHARED lib/EmxProtoV1.cpp)
strip(${TARGET_LIB} lib${TARGET_LIB}.so)
target_include_directories(${TARGET_LIB} PRIVATE ../net/include)
target_include_directories(${TARGET_LIB} PRIVATE ../comEnv/include)
target_include_directories(${TARGET_LIB} PRIVATE ../sdcard/include)
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
