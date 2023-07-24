cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(inner)

set(TARGET_APP DevTools)
aux_source_directory(. SRCS_APP)
add_executable(${TARGET_APP} ${SRCS_APP})
strip(${TARGET_APP} ${TARGET_APP})

#link
target_link_libraries(${TARGET_APP} ${TARGET_LIB} EmxCore libjsoncpp.so libuv.so pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})
