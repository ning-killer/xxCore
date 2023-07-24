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
        message(FATAL_ERROR "EmxMedia:${name} not be specified")
    else ()
        message(STATUS "EmxMedia:${name}=${${name}}")
    endif ()
endmacro()

check(CROSS)
check(MediaProfile)
check(ThirdPartyPath)
check(CorePath)
check(INSTALL_MEDIA_BIN)
check(INSTALL_APP_LIB)
check(JSONCPP_VERSION)
check(LIBUV_VERSION)
check(FDKAAC_VERSION)
check(ZBAR_VERSION)
check(FREETYPE_VERSION)
check(ZLIB_VERSION)

# 用来build之后strip的宏
macro(strip target filename)
    add_custom_command(
            TARGET
            ${target}
            POST_BUILD
            COMMAND ${CMAKE_STRIP} -s ${filename}
            COMMENT "Strip ${filename} done")
endmacro()

# include the specified profile cmake
include(profile/${MediaProfile}.cmake)

include_directories(inner)
include_directories(include)

# jsoncpp
link_directories(${ThirdPartyPath}/jsoncpp/${JSONCPP_VERSION}/${CROSS}/lib)
include_directories(${ThirdPartyPath}/jsoncpp/${JSONCPP_VERSION}/${CROSS}/include)
# libuv
link_directories(${ThirdPartyPath}/libuv/${LIBUV_VERSION}/${CROSS}/lib)
include_directories(${ThirdPartyPath}/libuv/${LIBUV_VERSION}/${CROSS}/include)

# core
include_directories(${CorePath}/include)


# 定义Server使用的profile相关的宏定义
add_definitions(-DMEDIA_PROFILE=${MediaProfile})
add_definitions(-DMEDIA_PROFILE_FACTORY=${MediaProfile}Factory)
add_definitions(-DMEDIA_PROFILE_HEADER="${MediaProfile}Factory.hpp")

# 添加源文件
aux_source_directory(app/server APP_SRCS)
include_directories(app/server)
aux_source_directory(app/audio APP_SRCS)
include_directories(app/audio)
aux_source_directory(app/osd APP_SRCS)
include_directories(app/osd)
aux_source_directory(app/yuvProc APP_SRCS)
include_directories(app/yuvProc)
aux_source_directory(app/screenExposure APP_SRCS)
include_directories(app/screenExposure)
aux_source_directory(app/raw2Jpge APP_SRCS)
include_directories(app/raw2Jpge)
aux_source_directory(app/panoQrPreProcess APP_SRCS)
include_directories(app/panoQrPreProcess)
aux_source_directory(app/region APP_SRCS)
include_directories(app/region)
aux_source_directory(app/platform/${HARDWARE_PLATFORM}/${CHIP} APP_SRCS)
include_directories(app/platform/${HARDWARE_PLATFORM}/${CHIP})

# add chip sample code if exist
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/app/platform/${HARDWARE_PLATFORM}/${CHIP}/sample)
    aux_source_directory(app/platform/${HARDWARE_PLATFORM}/${CHIP}/sample APP_SRCS)
    include_directories(app/platform/${HARDWARE_PLATFORM}/${CHIP}/sample)
endif ()

# add chip profile
file(GLOB CHIP_PROFILE_DIR app/platform/${HARDWARE_PLATFORM}/${CHIP}/profile/*)
FOREACH (profile ${CHIP_PROFILE_DIR})
    aux_source_directory(${profile} APP_SRCS)
    include_directories(${profile})
    message("add chip profile ${profile}")
ENDFOREACH (profile)

link_directories(${ThirdPartyPath}/fdk-aac/${FDKAAC_VERSION}/${CROSS}/lib)
include_directories(${ThirdPartyPath}/fdk-aac/${FDKAAC_VERSION}/${CROSS}/include)
set(EXT_LIBS ${EXT_LIBS} fdk-aac.a)

# add zbar
include_directories(${ThirdPartyPath}/zbar/${ZBAR_VERSION}/${CROSS}/include)
link_directories(${ThirdPartyPath}/zbar/${ZBAR_VERSION}/${CROSS}/lib)
set(EXT_LIBS ${EXT_LIBS} libzbar.a)

# add freetype
include_directories(${ThirdPartyPath}/freetype/${FREETYPE_VERSION}/${CROSS}/include)
include_directories(${ThirdPartyPath}/freetype/${FREETYPE_VERSION}/${CROSS}/include/freetype2)
link_directories(${ThirdPartyPath}/freetype/${FREETYPE_VERSION}/${CROSS}/lib)
set(EXT_LIBS ${EXT_LIBS} libfreetype.a)

# add z
include_directories(${ThirdPartyPath}/zlib/${ZLIB_VERSION}/${CROSS}/include)
link_directories(${ThirdPartyPath}/zlib/${ZLIB_VERSION}/${CROSS}/lib)
set(EXT_LIBS ${EXT_LIBS} libz.a)

#libturbojpeg
include_directories(${ThirdPartyPath}/libturbojpeg/${TURBOJPEG}/${CROSS}/include)
link_directories(${ThirdPartyPath}/libturbojpeg/${TURBOJPEG}/${CROSS}/lib)

# media compile_options,libs,link_dir
# fill MEDIA_LIBS MEDIA_LIB_DIR MEDIA_INC_DIR
if (EXISTS ${ThirdPartyPath}/chip/${HARDWARE_PLATFORM}/${CHIP}/${SDK_VERSION})
    include(${ThirdPartyPath}/chip/${HARDWARE_PLATFORM}/${CHIP}/${SDK_VERSION}/configure.cmake)
    FOREACH (ITEM ${MEDIA_SRC})
        aux_source_directory(${ThirdPartyPath}/chip/${HARDWARE_PLATFORM}/${CHIP}/${SDK_VERSION}/${ITEM} APP_SRCS)
    ENDFOREACH (ITEM)
    FOREACH (ITEM ${MEDIA_INC_DIR})
        include_directories(${ThirdPartyPath}/chip/${HARDWARE_PLATFORM}/${CHIP}/${SDK_VERSION}/${ITEM})
    ENDFOREACH (ITEM)
    FOREACH (ITEM ${MEDIA_LIB_DIR})
        link_directories(${ThirdPartyPath}/chip/${HARDWARE_PLATFORM}/${CHIP}/${SDK_VERSION}/${ITEM})
    ENDFOREACH (ITEM)
endif ()

# 编译Server应用程序
message(================APP_SRCS-BEGIN================)
FOREACH (src ${APP_SRCS})
    message(${src})
ENDFOREACH (src)
message(================APP_SRCS-END================)
add_executable(EmxMediaServer ${APP_SRCS})
target_link_libraries(EmxMediaServer ${MEDIA_LIBS} ${EXT_LIBS} EmxMedia EmxCore libjsoncpp.so libturbojpeg.a libuv.so m dl pthread)
strip(EmxMediaServer EmxMediaServer)
install(TARGETS EmxMediaServer DESTINATION ${INSTALL_MEDIA_BIN})


#libEmxMedia.so
aux_source_directory(lib LIB_SRCS)
add_library(EmxMedia SHARED ${LIB_SRCS})
strip(EmxMedia libEmxMedia.so)
install(TARGETS EmxMedia DESTINATION ${INSTALL_APP_LIB})
