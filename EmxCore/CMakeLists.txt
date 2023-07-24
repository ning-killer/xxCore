cmake_minimum_required(VERSION 3.10)

# 通用的编译选项
add_compile_options(
        -g
        -Wall
        -O2
        -fPIC
        -Werror
        -Wno-unused-result
        -Wno-format-truncation
        -Wno-stringop-truncation
        -Wno-format-overflow)

# check define
macro(check name)
    if (NOT ${name})
        message(FATAL_ERROR "EmxCore:${name} not be specified")
    else ()
        message(STATUS "EmxCore:${name}=${${name}}")
    endif ()
endmacro()

check(CROSS)
check(ThirdPartyPath)
check(INSTALL_APP_BIN)
check(INSTALL_APP_LIB)
check(JSONCPP_VERSION)
check(LIBUV_VERSION)

# 用来build之后strip的宏
macro(strip target filename)
    add_custom_command(
            TARGET
            ${target}
            POST_BUILD
            COMMAND ${CMAKE_STRIP} -s ${filename}
            COMMENT "Strip ${filename} done")
endmacro()

include_directories(inner)
include_directories(include)

link_directories(${ThirdPartyPath}/jsoncpp/${JSONCPP_VERSION}/${CROSS}/lib)
include_directories(${ThirdPartyPath}/jsoncpp/${JSONCPP_VERSION}/${CROSS}/include)
link_directories(${ThirdPartyPath}/libuv/${LIBUV_VERSION}/${CROSS}/lib)
include_directories(${ThirdPartyPath}/libuv/${LIBUV_VERSION}/${CROSS}/include)

# build libEmxCore.so
aux_source_directory(lib SRCS_LIB)
aux_source_directory(lib/euv SRCS_LIB)
aux_source_directory(lib/log SRCS_LIB)
aux_source_directory(lib/msg SRCS_LIB)
aux_source_directory(lib/aes SRCS_LIB)
aux_source_directory(lib/param SRCS_LIB)
aux_source_directory(lib/gpio SRCS_LIB)
aux_source_directory(lib/utils SRCS_LIB)
add_library(EmxCore SHARED ${SRCS_LIB})
strip(EmxCore libEmxCore.so)
install(TARGETS EmxCore DESTINATION ${INSTALL_APP_LIB})

# build EmxCoreServer
aux_source_directory(app SRCS_APP)
add_executable(EmxCoreServer ${SRCS_APP})
target_link_libraries(EmxCoreServer EmxCore libjsoncpp.so libuv.so m pthread)
strip(EmxCoreServer EmxCoreServer)
install(TARGETS EmxCoreServer DESTINATION ${INSTALL_APP_BIN})

