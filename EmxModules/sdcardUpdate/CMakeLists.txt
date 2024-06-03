cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(inner)
include_directories(app)
include_directories(../sdcard/include)
include_directories(../update/include)
include_directories(../comEnv/include)
include_directories(${MediaPath}/include)


set(TARGET_APP_LIB SDCardUpdateLib)
aux_source_directory(app SRCS_APP)
add_library(${TARGET_APP_LIB} STATIC ${SRCS_APP})
install(TARGETS ${TARGET_APP_LIB} DESTINATION ${INSTALL_APP_LIB})


set(TARGET_APP SDCardUpdate)
add_executable(${TARGET_APP} main.cpp)
strip(${TARGET_APP} ${TARGET_APP})

#link
target_link_libraries(${TARGET_APP} ${TARGET_APP_LIB} ComEnv Update SDCard EmxMedia EmxCore libjsoncpp.so libuv.so pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/sdcardUpdate)
