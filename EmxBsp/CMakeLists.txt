cmake_minimum_required(VERSION 3.10)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_CXX_STANDARD 11)

# check define
macro(check name)
    if (NOT ${name})
        message(FATAL_ERROR "EmxBsp:${name} not be specified")
    else ()
        message(STATUS "EmxBsp:${name}=${${name}}")
    endif ()
endmacro()

check(CROSS)
check(ThirdPartyPath)
check(CorePath)
check(ModulesPath)
check(Board)
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

set(TARGET_LIB BspImpl)
aux_source_directory(lib LIB_SRCS)
aux_source_directory(board/${Board} LIB_SRCS)
add_library(${TARGET_LIB} SHARED ${LIB_SRCS})
set_target_properties(${TARGET_LIB} PROPERTIES
        OUTPUT_NAME Bsp
        POSITION_INDEPENDENT_CODE ON
        )
target_include_directories(${TARGET_LIB} PRIVATE include)
# core
target_include_directories(${TARGET_LIB} PRIVATE ${CorePath}/include)
# modules bsp
target_include_directories(${TARGET_LIB} PRIVATE ${ModulesPath}/bsp/include)

#thirdparty
include_directories(${ThirdPartyPath}/jsoncpp/${JSONCPP_VERSION}/${CROSS}/include)
include_directories(${ThirdPartyPath}/libuv/${LIBUV_VERSION}/${CROSS}/include)

strip(${TARGET_LIB} libBsp.so)
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})
