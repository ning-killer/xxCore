cmake_minimum_required(VERSION 3.10)


include_directories(include)
include_directories(app)
include_directories(app/fileService)
include_directories(../net/include)
include_directories(../comEnv/include)
include_directories(../emxProtoV2/include)
include_directories(../update/include)

set(TARGET_APP_LIB EmxToolsV2Lib)
aux_source_directory(app SRCS_APP)
aux_source_directory(app/fileService SRCS_APP)
add_library(${TARGET_APP_LIB} STATIC ${SRCS_APP})
install(TARGETS ${TARGET_APP_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP EmxToolsV2)
add_executable(${TARGET_APP} main.cpp)
strip(${TARGET_APP} ${TARGET_APP})

target_link_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/lib)


#link
target_link_libraries(${TARGET_APP}
        ${TARGET_APP_LIB}
        ${TARGET_LIB}
        Update
        RecordStd
        Event
        Schedule
        EmxProtoV2
        Net
        ComEnv
        SDCard
        NightVision
        EmxCrypto
        EmxMedia
        EmxCore
        libjsoncpp.so
        libuv.so
        m pthread)

if (${SSL} STREQUAL "openssl")
    target_link_libraries(${TARGET_APP} ssl crypto)
elseif (${SSL} STREQUAL "mbedtls")
    target_link_libraries(${TARGET_APP} mbedtls mbedx509 mbedcrypto)
else ()
    message(FATAL SSL=${SSL})
endif ()

#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/emxTools)
