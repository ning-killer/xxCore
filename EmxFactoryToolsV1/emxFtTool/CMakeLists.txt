cmake_minimum_required(VERSION 3.10)

set(TARGET_APP EmxFctToolsV1)
aux_source_directory(app SRCS_APP)
# aux_source_directory(app/FactoryProto SRCS_APP)
add_executable(${TARGET_APP} ${SRCS_APP})
strip(${TARGET_APP} ${TARGET_APP})
target_include_directories(${TARGET_APP} PRIVATE app)
# target_include_directories(${TARGET_APP} PRIVATE app/FactoryProto)
target_include_directories(${TARGET_APP} PRIVATE inner)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/net/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/comEnv/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/sdcard/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/nightVision/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/bsp/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/update/include)
target_include_directories(${TARGET_APP} PRIVATE ${ModulesPath}/emxCrypto/include)
target_include_directories(${TARGET_APP} PRIVATE ${BspPath}/include)
target_include_directories(${TARGET_APP} PRIVATE ${FactoryPath}/emxAgeingTool/inner)
#ssl
target_include_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/include)
target_link_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/${SSL}/${SSL_VERSION}/${CROSS}/lib)

#link
target_link_libraries(
        ${TARGET_APP}
        EmxCrypto
        libmbedtls.so
        libmbedx509.so
        libmbedcrypto.so
        Update 
        ComEnv
        BspImpl 
        Bsp
        NightVision
        Net 
        SDCard 
        EmxMedia 
        EmxCore
        libconfig.a
        libjsoncpp.so 
        libuv.so 
        pthread
)

#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})
