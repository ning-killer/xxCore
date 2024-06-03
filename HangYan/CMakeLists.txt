cmake_minimum_required(VERSION 3.10)

set(TARGET EapilMain)

# check define
macro(check name)
    if (NOT ${name})
        message(FATAL_ERROR "HangYan:${name} not be specified")
    else ()
        message(STATUS "HangYan:${name}=${${name}}")
    endif ()
endmacro()

check(MediaPath)
check(CorePath)
check(ModulesPath)
check(ThirdPartyPath)
check(OvdVersion)
check(AndLinkVersion)
check(VoipVersion)
check(VoipApiVersion)
check(CROSS)
check(INSTALL_APP_BIN)
check(JSONCPP_VERSION)
check(LIBUV_VERSION)

if (MergeProcess MATCHES ON)
    add_definitions(-DMERGE_PROCESS)
ENDIF()

add_definitions(
    -DTestDef=1
)

#小系统编译
if (EAPIL_SPARE)
    add_definitions(-DEAPIL_SPARE)
endif ()

ADD_DEFINITIONS(
    -DOnlySupportDHCP # 仅支持dhcp
)

# 通用的编译选项
add_compile_options(
        -g
        -Wall
        -O2
        -fPIC
        -Werror
        -Wno-permissive
        -Wno-unused-result
        -Wno-format-truncation
        -Wno-stringop-truncation
        -Wno-format-overflow)

# 需要屏蔽掉这个文件内2B操作引发的告警
set_source_files_properties(
        source/voip/${VoipApiVersion}/third_party_device.cc
        PROPERTIES
        COMPILE_FLAGS "-fpermissive -Wno-error -w"
)

# 需要屏蔽掉这个文件内2B操作引发的告警
# 头文件EapilThirdParty/platform/HangYan/cmcc_voip/1.5.3.1/include/common_types.h:907 audioLevel成员变量初始化顺序
set_source_files_properties(
        source/voip/${VoipApiVersion}/third_party_device_factory.cc
        source/voip/${VoipApiVersion}/cmcc_voip.cc
        PROPERTIES
        COMPILE_FLAGS "-Wno-reorder"
)

#src
include_directories(include)
aux_source_directory(source SRC)
include_directories(source)
aux_source_directory(source/callback SRC)
include_directories(source/callback)
aux_source_directory(source/led SRC)
include_directories(source/led)
aux_source_directory(source/env SRC)
include_directories(source/env)
aux_source_directory(source/net SRC)
include_directories(source/net)
aux_source_directory(source/voice SRC)
include_directories(source/voice)
if (NOT EAPIL_SPARE)
    aux_source_directory(source/andlink SRC)
    include_directories(source/andlink)
    aux_source_directory(source/voip SRC)
    include_directories(source/voip)
    aux_source_directory(source/voip/${VoipApiVersion} SRC)
    include_directories(source/voip/${VoipApiVersion})
    aux_source_directory(source/alarm SRC)
    include_directories(source/alarm)
    aux_source_directory(source/autoMaintain SRC)
    include_directories(source/autoMaintain)
    aux_source_directory(source/avPush SRC)
    include_directories(source/avPush)
    aux_source_directory(source/nightVision SRC)
    include_directories(source/nightVision)
    aux_source_directory(source/record SRC)
    include_directories(source/record)
    aux_source_directory(source/schedule SRC)
    include_directories(source/schedule)
    aux_source_directory(source/softProbe SRC)
    include_directories(source/softProbe)
    aux_source_directory(source/gat1400 SRC)
    include_directories(source/gat1400)
    aux_source_directory(source/gat1400/curlclient SRC)
    include_directories(source/gat1400/curlclient)
endif ()

add_executable(${TARGET} ${SRC})
# 用来build之后strip的宏
macro(strip target filename)
    add_custom_command(
            TARGET
            ${target}
            POST_BUILD
            COMMAND ${CMAKE_STRIP} -s ${filename}
            COMMENT "Strip ${filename} done")
endmacro()

strip(${TARGET} ${TARGET})

# bsp
target_include_directories(${TARGET} PRIVATE ${BspPath}/include)

# media
target_include_directories(${TARGET} PRIVATE ${MediaPath}/include)

# core
target_include_directories(${TARGET} PRIVATE ${CorePath}/include)

# modules
target_include_directories(${TARGET} PRIVATE ${ModulesPath}/threadInvoke/include)
target_include_directories(${TARGET} PRIVATE ${ModulesPath}/comEnv/include)
target_include_directories(${TARGET} PRIVATE ${ModulesPath}/net/include)
target_include_directories(${TARGET} PRIVATE ${ModulesPath}/zone/include)
target_include_directories(${TARGET} PRIVATE ${ModulesPath}/update/include)
target_include_directories(${TARGET} PRIVATE ${ModulesPath}/sdcard/include)
target_include_directories(${TARGET} PRIVATE ${ModulesPath}/mediaFormat/include)
target_include_directories(${TARGET} PRIVATE ${ModulesPath}/key/include)
target_include_directories(${TARGET} PRIVATE ${ModulesPath}/nightVision/include)

if (MergeProcess MATCHES ON)
    target_include_directories(${TARGET} PRIVATE ${ModulesPath}/rtsp/include)
    target_include_directories(${TARGET} PRIVATE ${ModulesPath}/ntp/include)
    #wpa
    target_link_directories(${TARGET} PRIVATE ${ThirdPartyPath}/wpa_supplicant/${WPASUPPLICANT_VERSION}/${CROSS}/lib)
ENDIF()

# factory
target_include_directories(${TARGET} PRIVATE ${FactoryPath}/emxFtTool/inner)
target_include_directories(${TARGET} PRIVATE ${FactoryPath}/emxAgeingTool/inner)

#thirdparty

#libconfig
target_include_directories(${TARGET} PRIVATE ${ThirdPartyPath}/libconfig/${LIBCONFIG_VERSION}/${CROSS}/include)
target_link_directories(${TARGET} PRIVATE ${ThirdPartyPath}/libconfig/${LIBCONFIG_VERSION}/${CROSS}/lib)

#jsoncpp
target_include_directories(${TARGET} PRIVATE ${ThirdPartyPath}/jsoncpp/${JSONCPP_VERSION}/${CROSS}/include)
target_link_directories(${TARGET} PRIVATE ${ThirdPartyPath}/jsoncpp/${JSONCPP_VERSION}/${CROSS}/lib)
#libuv
target_include_directories(${TARGET} PRIVATE ${ThirdPartyPath}/libuv/${LIBUV_VERSION}/${CROSS}/include)
target_link_directories(${TARGET} PRIVATE ${ThirdPartyPath}/libuv/${LIBUV_VERSION}/${CROSS}/lib)
#curl
target_include_directories(${TARGET} PRIVATE ${ThirdPartyPath}/curl/${CURL_VERSION}/${SSL}/${SSL_VERSION}/${CROSS}/include)
target_link_directories(${TARGET} PRIVATE ${ThirdPartyPath}/curl/${CURL_VERSION}/${SSL}/${SSL_VERSION}/${CROSS}/lib)
#ssl
target_include_directories(${TARGET} PRIVATE ${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/include)
target_link_directories(${TARGET} PRIVATE ${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/lib)
#platform
set(PlatSDKPath ${ThirdPartyPath}/platform/HangYan)
#andlink
if (EXISTS ${PlatSDKPath}/andlink/${AndLinkVersion})
    FOREACH (ITEM ${ANDLINK_INC_DIR})
        target_include_directories(${TARGET} PRIVATE ${PlatSDKPath}/andlink/${AndLinkVersion}/${ITEM})
    ENDFOREACH (ITEM)
    FOREACH (ITEM ${ANDLINK_LIB_DIR})
        target_link_directories(${TARGET} PRIVATE ${PlatSDKPath}/andlink/${AndLinkVersion}/${ITEM}/${CROSS})
    ENDFOREACH (ITEM)
endif ()
#voip
if (EXISTS ${PlatSDKPath}/cmcc_voip/${VoipVersion})
    FOREACH (ITEM ${VOIP_INC_DIR})
        target_include_directories(${TARGET} PRIVATE ${PlatSDKPath}/cmcc_voip/${VoipVersion}/${ITEM})
    ENDFOREACH (ITEM)
    FOREACH (ITEM ${VOIP_LIB_DIR})
        target_link_directories(${TARGET} PRIVATE ${PlatSDKPath}/cmcc_voip/${VoipVersion}/${ITEM}/${CROSS})
    ENDFOREACH (ITEM)
endif ()
#ovd
if (EXISTS ${PlatSDKPath}/ovdsdk/${OvdVersion})
    FOREACH (ITEM ${OVDSDK_INC_DIR})
        target_include_directories(${TARGET} PRIVATE ${PlatSDKPath}/ovdsdk/${OvdVersion}/${ITEM})
    ENDFOREACH (ITEM)
    FOREACH (ITEM ${OVDSDK_LIB_DIR})
        target_link_directories(${TARGET} PRIVATE ${PlatSDKPath}/ovdsdk/${OvdVersion}/${ITEM}/${CROSS})
    ENDFOREACH (ITEM)
endif ()


if (MergeProcess MATCHES ON)
    SET(MERGE_PROCESS_LIBS
        libwpa_client.so
        BootEnv
        UpdateServerLib
        NetServerLib
        SDCardServerLib
        NightVisionServerLib
        RtspServerLib
        # NtpClientLib
    )
elseif (MergeProcess MATCHES OFF)
    SET(MERGE_PROCESS_LIBS
    )
endif()

target_link_libraries(EapilMain
        ${PRODUCT_LIB}
        ThreadInvoke
        Bsp
        Zone
        ComEnv
        Update
        Net
        SDCard
        Key
        NightVision
        EmxMedia
        EmxCore
        libconfig.a
        libjsoncpp.so
        libuv.so
        pthread
        ${VOIP_LIBS}
        ${ANDLINK_LIBS}
        curl
        ${OVDSDK_LIBS}
        ${MERGE_PROCESS_LIBS}
        libmbedtls.so
        libmbedx509.so
        libmbedcrypto.so
        m 
        dl
        )
if (NOT EAPIL_SPARE)
    target_link_libraries(EapilMain
            Key
            Mp4
            NightVision
            )
endif()
#install
install(TARGETS ${TARGET} DESTINATION ${INSTALL_APP_BIN})