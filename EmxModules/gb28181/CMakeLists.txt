cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(inner)

#set(TARGET_LIB EmxGB28181)
#aux_source_directory(lib SRCS_LIB)
#add_library(${TARGET_LIB} SHARED ${SRCS_LIB})
#strip(${TARGET_LIB} lib${TARGET_LIB}.so)
#
##install
#install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

#  warning "limits.h is not found or not supported. LONG_MIN and LONG_MAX "
set_source_files_properties(
        app/SCall.cpp
        app/RegClient.cpp
        app/PlaybackHandler.cpp
        app/PlayHandler.cpp
        app/SMessage.cpp
        app/SipApp.cpp
        app/SipUAC.cpp
        app/main.cpp
        PROPERTIES
        COMPILE_FLAGS "-Wno-cpp"
)

set(TARGET_APP GB28181Service)
aux_source_directory(app SRCS_APP)
add_executable(${TARGET_APP} ${SRCS_APP})
strip(${TARGET_APP} ${TARGET_APP})
target_include_directories(${TARGET_APP} PRIVATE app)
#sip
target_include_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/pjsip/${PJSIP_VERSION}/${CROSS}/include)
target_link_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/pjsip/${PJSIP_VERSION}/${CROSS}/lib)

target_compile_definitions(${TARGET_APP} PRIVATE
        -DPJ_IS_LITTLE_ENDIAN=1
        -DPJ_IS_BIG_ENDIAN=0
        )

#link
target_link_libraries(${TARGET_APP} ${TARGET_LIB}
        pjsip-${CROSS}
        pj-${CROSS}
        pjlib-util-${CROSS}
        EmxMedia EmxCore libjsoncpp.so libuv.so
        m pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/gb28181)

