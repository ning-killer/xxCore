cmake_minimum_required(VERSION 3.10)

include_directories(app)
include_directories(include)
include_directories(../emxProtoV2/include)
include_directories(../mediaFormat/include)
include_directories(${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/include)

set(TARGET_APP_LIB WebServerLib)
aux_source_directory(app SRCS_APP)
add_library(${TARGET_APP_LIB} STATIC ${SRCS_APP})
install(TARGETS ${TARGET_APP_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP WebServer)

#-Werror=strict-aliasing
# 需要屏蔽掉这个文件内2B操作引发的告警
set_source_files_properties(
        app/mongoose.c
        PROPERTIES
        COMPILE_FLAGS "-fpermissive -Wno-strict-aliasing -w"
)

add_executable(${TARGET_APP} main.cpp)
strip(${TARGET_APP} ${TARGET_APP})
target_link_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/lib)
#link
target_link_libraries(${TARGET_APP} ${TARGET_APP_LIB}
        Rtp RecordStd Event Schedule EmxProtoV2 Net ComEnv SDCard NightVision
        EmxMedia EmxCore EmxCrypto libjsoncpp.so libuv.so
        m pthread)

if (${SSL} STREQUAL "openssl")
    target_link_libraries(${TARGET_APP} ${TARGET_LIB}
            ssl crypto)
elseif (${SSL} STREQUAL "mbedtls")
    target_link_libraries(${TARGET_APP} ${TARGET_LIB}
            mbedtls mbedx509 mbedcrypto)
else ()
    message(FATAL SSL=${SSL})
endif ()

#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/webserver)
