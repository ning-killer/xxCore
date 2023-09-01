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
        -Wno-format-overflow
        -Wno-misleading-indentation)

# check define
macro(check name)
    if (NOT ${name})
        message(FATAL_ERROR "EmxModules:${name} not be specified")
    else ()
        message(STATUS "EmxModules:${name}=${${name}}")
    endif ()
endmacro()

check(CROSS)
check(ThirdPartyPath)
check(CorePath)
check(MediaPath)
check(ModulesPath)
check(INSTALL_APP_BIN)
check(INSTALL_APP_LIB)
check(JSONCPP_VERSION)
check(LIBUV_VERSION)
check(BspPath)
check(FactoryPath)

# 供所有的subdirectory使用
include_directories(${CorePath}/include)
include_directories(${MediaPath}/include)

# thirdparty
link_directories(${ThirdPartyPath}/jsoncpp/${JSONCPP_VERSION}/${CROSS}/lib)
include_directories(${ThirdPartyPath}/jsoncpp/${JSONCPP_VERSION}/${CROSS}/include)
link_directories(${ThirdPartyPath}/libuv/${LIBUV_VERSION}/${CROSS}/lib)
include_directories(${ThirdPartyPath}/libuv/${LIBUV_VERSION}/${CROSS}/include)
link_directories(${ThirdPartyPath}/libconfig/${LIBCONFIG_VERSION}/${CROSS}/lib)
include_directories(${ThirdPartyPath}/libconfig/${LIBCONFIG_VERSION}/${CROSS}/include)

#option(DO_STRIP "是否build之后对目标文件进行strip操作" )

# 用来build之后strip的宏
macro(strip target filename)
    add_custom_command(
            TARGET
            ${target}
            POST_BUILD
            COMMAND ${CMAKE_STRIP} -s ${filename}
            COMMENT "Strip ${filename} done")
endmacro()

add_subdirectory(emxFtTool)      # 产测工具
add_subdirectory(emxAgeingTool)  # 老化工具
