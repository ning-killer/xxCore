cmake_minimum_required(VERSION 3.10)

set(TARGET_APP AudioDemo)
aux_source_directory(. SRCS_APP)
add_executable(${TARGET_APP} ${SRCS_APP})
strip(${TARGET_APP} ${TARGET_APP})

target_include_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/fdk-aac/${FDKAAC_VERSION}/${CROSS}/include)
target_link_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/fdk-aac/${FDKAAC_VERSION}/${CROSS}/lib)

#link
target_link_libraries(${TARGET_APP} EmxCore fdk-aac.a libjsoncpp.so libuv.so pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})
