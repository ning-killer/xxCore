cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(inner)

include_directories(../schedule/include)
include_directories(../event/include)
include_directories(../mediaFormat/include)
include_directories(app)
include_directories(../sdcard/include)

set(TARGET_LIB RecordStd)
aux_source_directory(lib SRCS_LIB)
add_library(${TARGET_LIB} SHARED ${SRCS_LIB})
strip(${TARGET_LIB} lib${TARGET_LIB}.so)

#install
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP_LIB RecordStdServerLib)
aux_source_directory(app SRCS_APP)
add_library(${TARGET_APP_LIB} STATIC ${SRCS_APP})
install(TARGETS ${TARGET_APP_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP RecordStdServer)
add_executable(${TARGET_APP} main.cpp)
strip(${TARGET_APP} ${TARGET_APP})


#link
target_link_libraries(${TARGET_APP} ${TARGET_APP_LIB} ${TARGET_LIB} Mp4 Event Schedule SDCard EmxMedia EmxCore libjsoncpp.so libuv.so pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/net)

