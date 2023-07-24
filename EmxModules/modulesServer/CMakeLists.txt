cmake_minimum_required(VERSION 3.10)

set(TARGET_APP EmxModulesServer)
aux_source_directory(app SRCS_APP)
add_executable(${TARGET_APP} ${SRCS_APP})
strip(${TARGET_APP} ${TARGET_APP})
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/comEnv/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/update/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/sdcard/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/nightVision/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/net/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/rtsp/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/ntp/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/sdcardUpdate/include)
#wpa
target_link_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/wpa_supplicant/${WPASUPPLICANT_VERSION}/${CROSS}/lib)
#libconfig
target_include_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/libconfig/include)
target_link_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/libconfig/lib/${CROSS})
# factory
target_include_directories(${TARGET_APP} PRIVATE ${FactoryPath}/emxFtTool/inner)
target_include_directories(${TARGET_APP} PRIVATE ${FactoryPath}/emxAgeingTool/inner)
#link
target_link_libraries(
    ${TARGET_APP}
    Bsp
    Zone
    Update
    Net
    SDCard
    NightVision
    BootEnv
    libwpa_client.so
    UpdateServerLib
    NetServerLib
    SDCardServerLib
    NightVisionServerLib
    RtspServerLib
    NtpClientLib
    SDCardUpdateLib
    ComEnv
    EmxCore
    EmxMedia
    libconfig.a
    libjsoncpp.so 
    libuv.so 
    pthread
)

#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})