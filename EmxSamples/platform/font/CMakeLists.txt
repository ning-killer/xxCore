cmake_minimum_required(VERSION 3.10)

check(FREETYPE_VERSION)

set(TARGET_APP FontDemo)
aux_source_directory(. SRCS_APP)
add_executable(${TARGET_APP} ${SRCS_APP})
strip(${TARGET_APP} ${TARGET_APP})

# add freetype
target_include_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/freetype/${FREETYPE_VERSION}/${CROSS}/include)
target_include_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/freetype/${FREETYPE_VERSION}/${CROSS}/include/freetype2)
target_link_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/freetype/${FREETYPE_VERSION}/${CROSS}/lib)

#link
target_link_libraries(${TARGET_APP} EmxCore libfreetype.a libjsoncpp.so libuv.so pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})
