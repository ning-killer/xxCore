cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(../net/include)

set(TARGET_APP_LIB NtpClientLib)
aux_source_directory(app SRCS_APP)
add_library(${TARGET_APP_LIB} STATIC ${SRCS_APP})
install(TARGETS ${TARGET_APP_LIB} DESTINATION ${INSTALL_APP_LIB})


set(TARGET_APP NtpClient)
add_executable(${TARGET_APP} main.cpp)
strip(${TARGET_APP} ${TARGET_APP})

target_link_directories(${TARGET_APP} PRIVATE ../net/lib)
#link
target_link_libraries(${TARGET_APP} ${TARGET_APP_LIB} ${TARGET_LIB} EmxCore libjsoncpp.so libuv.so pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/ntp)
