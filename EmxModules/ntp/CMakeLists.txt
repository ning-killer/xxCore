cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(inner)
include_directories(../net/include)

set(TARGET_APP_LIB NtpServerLib)
aux_source_directory(app SRCS_APP)
add_library(${TARGET_APP_LIB} STATIC ${SRCS_APP})
if (InstallAppServerStaticLib)
    install(TARGETS ${TARGET_APP_LIB} DESTINATION ${INSTALL_APP_LIB})
endif ()

set(TARGET_LIB NtpClient)
aux_source_directory(lib SRCS_LIB)
add_library(${TARGET_LIB} SHARED ${SRCS_LIB})
strip(${TARGET_LIB} lib${TARGET_LIB}.so)
#install
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP NtpServer)
add_executable(${TARGET_APP} main.cpp)
strip(${TARGET_APP} ${TARGET_APP})

target_link_directories(${TARGET_APP} PRIVATE ../net/lib)
#link
target_link_libraries(${TARGET_APP} ${TARGET_APP_LIB} ${TARGET_LIB} EmxCore libjsoncpp.so libuv.so pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/ntp)
