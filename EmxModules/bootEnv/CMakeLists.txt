cmake_minimum_required(VERSION 3.10)

include_directories(include)

include_directories(${TARGET_LIB} PRIVATE include)
# core
include_directories(${TARGET_LIB} PRIVATE ${Release}/EmxCore/${CoreApiVersion}/include)
# modules
include_directories(${TARGET_LIB} PRIVATE ${Modules}/bsp/include)

#thirdparty
set(ThirdPartyPath ${Release}/EmxThirdParty)
include_directories(${ThirdPartyPath}/jsoncpp/${JSONCPP_VERSION}/${CROSS}/include)
include_directories(${ThirdPartyPath}/libuv/${LIBUV_VERSION}/${CROSS}/include)

set(TARGET_LIB BootEnv)
aux_source_directory(src/fw_env SRC)
add_library(${TARGET_LIB} SHARED ${SRC} src/BootEnv.cpp)
strip(${TARGET_LIB} lib${TARGET_LIB}.so)
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})
#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})

set(TARGET_APP BootEnvTool)
add_executable(${TARGET_APP} src/BootEnvTool.cpp)
strip(${TARGET_APP} ${TARGET_APP})
target_link_libraries(${TARGET_APP} ${TARGET_LIB} EmxCore libjsoncpp.so libuv.so pthread)
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/bootEnv)
