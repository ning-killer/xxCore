cmake_minimum_required(VERSION 3.10)

set(TARGET_LIB EmxCrypto)

include_directories(.)
include_directories(include)

add_library(${TARGET_LIB} SHARED lib/MbedtlsCrypto.cpp lib/OpensslCrypto.cpp)
strip(${TARGET_LIB} lib${TARGET_LIB}.so)
target_include_directories(${TARGET_LIB} PRIVATE ../emxCrypto/include)
#ssl
target_include_directories(${TARGET_LIB} PRIVATE ${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/include)
target_link_directories(${TARGET_LIB} PRIVATE ${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/lib)

install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
