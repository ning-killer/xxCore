cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(inner)
include_directories(../rtsp/include)
include_directories(../comEnv/include)
include_directories(${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/include)

set(TARGET_LIB Onvif)
aux_source_directory(lib SRCS_LIB)
add_library(${TARGET_LIB} SHARED ${SRCS_LIB})
strip(${TARGET_LIB} lib${TARGET_LIB}.so)

#install
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP_LIB OnvifServiceLib)
aux_source_directory(app SRCS_APP)
add_library(${TARGET_APP_LIB} STATIC ${SRCS_APP})
install(TARGETS ${TARGET_APP_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP OnvifService)
add_executable(${TARGET_APP} main.cpp)
strip(${TARGET_APP} ${TARGET_APP})
target_include_directories(${TARGET_APP} PRIVATE app)
#ssl
target_link_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/lib)

#屏蔽异常告警
target_compile_options(${TARGET_APP} PRIVATE -Wno-psabi)

#  warning ‘int RAND_pseudo_bytes(unsigned char*, int)’ is deprecated [-Werror=deprecated-declarations]
set_source_files_properties(
        app/soapUtil.cpp
        PROPERTIES
        COMPILE_FLAGS "-Wno-deprecated-declarations"
)

#link
target_link_libraries(${TARGET_APP} ${TARGET_APP_LIB} ${TARGET_LIB}
        EmxMedia EmxCore Rtsp libjsoncpp.so libuv.so)
if (${SSL} STREQUAL "openssl")
    target_link_libraries(${TARGET_APP} ${TARGET_LIB}
            ssl crypto)
elseif (${SSL} STREQUAL "mbedtls")
    target_link_libraries(${TARGET_APP} ${TARGET_LIB}
            mbedtls mbedx509 mbedcrypto)
else ()
    message(FATAL SSL=${SSL})
endif ()

target_link_libraries(${TARGET_APP} ${TARGET_LIB}
        m pthread ComEnv)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/onvif)

