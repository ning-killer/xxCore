# 这里定义版本依赖

# 服务进程合并开关 OFF关闭 ON开启
set(MergeProcess ON)

# 杭研依赖
set(PlatSDKPath ${ThirdPartyPath}/platform/HangYan)
include(${PlatSDKPath}/andlink/1.5.2/configure.cmake)
include(${PlatSDKPath}/cmcc_voip/1.5.12.2/configure.cmake)
include(${PlatSDKPath}/ovdsdk/1.28/configure.cmake)

#硬件依赖
set(Board EpCv182xGunV1)

#媒体依赖
set(MediaProfile cv182xFDPDWith)

#第三方库依赖
set(JSONCPP_VERSION 1.9.3)
set(LIBUV_VERSION 1.44.2)
set(WPASUPPLICANT_VERSION 2.9)
set(PJSIP_VERSION 0.0.0)
set(SSL mbedtls) #openssl/mbedtls
set(SSL_VERSION 2.28.1)
if (${SSL} STREQUAL openssl)
    add_definitions(-DEMX_SSL_OPENSSL)
elseif (${SSL} STREQUAL mbedtls)
    add_definitions(-DEMX_SSL_MBEDTLS)
else ()
    message(FATAL_ERROR "SSL not be specified")
endif ()
set(CURL_VERSION 7.72.0)
set(FDKAAC_VERSION 0.1.6)
set(ZBAR_VERSION 0.10)
set(FREETYPE_VERSION 2.4.10)
set(ZLIB_VERSION 1.2.11)
set(TURBOJPEG    1.5.3)

option(SUPPORT_EVENT_STD "是否支持modules中的event" ON)
if (SUPPORT_EVENT_STD)
    add_definitions(-DSUPPORT_EVENT_STD)
endif ()

option(SUPPORT_RECORD_STD "是否支持modules中的recordStd，使能后其依赖的event也会被打开" ON)
if (SUPPORT_RECORD_STD)
    add_definitions(-DSUPPORT_RECORD_STD)
    option(SUPPORT_EVENT_STD "是否支持modules中的event" ON)
    add_definitions(-DSUPPORT_EVENT_STD)
endif ()
