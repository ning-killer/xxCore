cmake_minimum_required(VERSION 3.10)
#工程名
project(CMSXJ40H)

# 编译链
set(CROSS arm-cvitek-linux-uclibcgnueabihf)
set(CMAKE_C_COMPILER ${CROSS_PATH}${CROSS}-gcc)
set(CMAKE_CXX_COMPILER ${CROSS_PATH}${CROSS}-g++)
set(CMAKE_STRIP ${CROSS_PATH}${CROSS}-strip)

# app安装目录
if (NOT INSTALL_APP_DIR)
    set(INSTALL_APP_DIR ${CMAKE_SOURCE_DIR}/firmware/app)
endif ()
set(INSTALL_APP_BIN ${INSTALL_APP_DIR}/bin)
set(INSTALL_APP_LIB ${INSTALL_APP_DIR}/lib)
# media安装目录
if (NOT INSTALL_MEDIA_DIR)
    set(INSTALL_MEDIA_DIR ${CMAKE_SOURCE_DIR}/firmware/media)
endif ()
set(INSTALL_MEDIA_BIN ${INSTALL_MEDIA_DIR}/bin)
set(INSTALL_MEDIA_LIB ${INSTALL_MEDIA_DIR}/lib)

set(ThirdPartyPath ${CMAKE_SOURCE_DIR}/EmxThirdParty)
set(CorePath ${CMAKE_SOURCE_DIR}/EmxCore)
set(MediaPath ${CMAKE_SOURCE_DIR}/EmxMedia)
set(ModulesPath ${CMAKE_SOURCE_DIR}/EmxModules)
set(BspPath ${CMAKE_SOURCE_DIR}/EmxBsp)
set(FactoryPath ${CMAKE_SOURCE_DIR}/EmxFactoryToolsV1)

# 加载依赖
include(depend.cmake)

# add_subdirectory
add_subdirectory(EmxCore)
add_subdirectory(EmxMedia)
if (MergeProcess MATCHES ON)
    SET(modules_server
        modulesServer
    )
elseif (MergeProcess MATCHES OFF)
    SET(modules_server
    )
endif()
set(ModuleList
        bootEnv
        bsp
        comEnv
        emxCrypto
        emxProtoV1
        emxToolsV1
        emxProtoV2
        emxToolsV2
        event
        key
        net
        ntp
        recordStd
        rtsp
        schedule
        sdcard
        taskManager
        threadInvoke
        zone
        mediaFormat
        update
        nightVision
        devTools
        sdcardUpdate
        # gb28181
        # onvif
        # webserver
        ${modules_server}
        )
add_subdirectory(EmxModules)
add_subdirectory(EmxBsp)
# add_subdirectory(HangYan)
add_subdirectory(EmxFactoryToolsV1)
add_subdirectory(EmxSamples)


