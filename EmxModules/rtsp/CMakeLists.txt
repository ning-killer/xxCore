cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(inner)
include_directories(app)

set(TARGET_LIB Rtsp)
aux_source_directory(lib SRCS_LIB)
add_library(${TARGET_LIB} SHARED ${SRCS_LIB})
strip(${TARGET_LIB} lib${TARGET_LIB}.so)

#install
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP_LIB RtspServerLib)
aux_source_directory(app SRCS_APP)
add_library(${TARGET_APP_LIB} STATIC ${SRCS_APP})
install(TARGETS ${TARGET_APP_LIB} DESTINATION ${INSTALL_APP_LIB})
target_compile_options(${TARGET_APP_LIB} PRIVATE -Wno-psabi)


set(TARGET_APP RtspServer)
add_executable(${TARGET_APP} main.cpp)
strip(${TARGET_APP} ${TARGET_APP})


#link
target_link_libraries(${TARGET_APP} ${TARGET_APP_LIB} ${TARGET_LIB}
        EmxMedia EmxCore libjsoncpp.so libuv.so
        m pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/rtsp)

