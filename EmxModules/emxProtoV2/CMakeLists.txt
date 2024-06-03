cmake_minimum_required(VERSION 3.10)

set(TARGET_LIB EmxProtoV2)

include_directories(.)
include_directories(include)

add_library(${TARGET_LIB} SHARED lib/EmxProtoV2.cpp)
strip(${TARGET_LIB} lib${TARGET_LIB}.so)
target_include_directories(${TARGET_LIB} PRIVATE ../net/include)
target_include_directories(${TARGET_LIB} PRIVATE ../comEnv/include)
target_include_directories(${TARGET_LIB} PRIVATE ../sdcard/include)
target_include_directories(${TARGET_LIB} PRIVATE ../nightVision/include)
target_include_directories(${TARGET_LIB} PRIVATE ../bsp/include)
target_include_directories(${TARGET_LIB} PRIVATE ../emxCrypto/include)
target_include_directories(${TARGET_LIB} PRIVATE ../zone/include)
target_include_directories(${TARGET_LIB} PRIVATE ../schedule/include)

target_include_directories(${TARGET_LIB} PRIVATE ../event/include)
target_include_directories(${TARGET_LIB} PRIVATE ../recordStd/include)
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
