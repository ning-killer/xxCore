cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(inner)

set(TARGET_LIB libTaskManager)
aux_source_directory(lib SRCS_LIB)
add_library(${TARGET_LIB} SHARED ${SRCS_LIB})
set_target_properties( ${TARGET_LIB} PROPERTIES
        OUTPUT_NAME TaskManager
        POSITION_INDEPENDENT_CODE ON
        )
strip(${TARGET_LIB} libTaskManager.so)
#modules
target_include_directories(${TARGET_LIB} PRIVATE ../bsp/include)
target_include_directories(${TARGET_LIB} PRIVATE ../schedule/include)
target_include_directories(${TARGET_LIB} PRIVATE ../zone/include)

#install
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP TaskManager)
aux_source_directory(app SRCS_APP)
add_executable(${TARGET_APP} ${SRCS_APP})
strip(${TARGET_APP} ${TARGET_APP})
target_include_directories(${TARGET_APP} PRIVATE app)
#modules
target_include_directories(${TARGET_APP} PRIVATE ../bsp/include)
target_include_directories(${TARGET_APP} PRIVATE ../schedule/include)
target_include_directories(${TARGET_APP} PRIVATE ../zone/include)
#link
target_link_libraries(${TARGET_APP} ${TARGET_LIB} Bsp Zone Schedule EmxMedia EmxCore libjsoncpp.so libuv.so pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/net)

