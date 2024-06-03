cmake_minimum_required(VERSION 3.10)

include_directories(include)
include_directories(inner)
include_directories(app)
include_directories(app/link)
include_directories(app/dev)
include_directories(app/dev/eth)
include_directories(app/dev/wifi)
include_directories(app/dev/mobile)
include_directories(${ThirdPartyPath}/wpa_supplicant/${WPASUPPLICANT_VERSION}/${CROSS}/include)
include_directories(${ModulesPath}/comEnv/include)

set(TARGET_LIB Net)
aux_source_directory(lib SRCS_LIB)
add_library(${TARGET_LIB} SHARED ${SRCS_LIB})
strip(${TARGET_LIB} lib${TARGET_LIB}.so)

#install
install(TARGETS ${TARGET_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP_LIB NetServerLib)
aux_source_directory(app SRCS_APP)
aux_source_directory(app/link SRCS_APP)
aux_source_directory(app/dev SRCS_APP)
aux_source_directory(app/dev/eth SRCS_APP)
aux_source_directory(app/dev/wifi SRCS_APP)
aux_source_directory(app/dev/mobile SRCS_APP)
add_library(${TARGET_APP_LIB} STATIC ${SRCS_APP})
install(TARGETS ${TARGET_APP_LIB} DESTINATION ${INSTALL_APP_LIB})

set(TARGET_APP NetServer)

add_executable(${TARGET_APP} main.cpp)
strip(${TARGET_APP} ${TARGET_APP})

#wpa
target_link_directories(${TARGET_APP} PRIVATE ${ThirdPartyPath}/wpa_supplicant/${WPASUPPLICANT_VERSION}/${CROSS}/lib)
#link
target_link_libraries(${TARGET_APP} ${TARGET_APP_LIB} ${TARGET_LIB} ComEnv libwpa_client.so Net EmxCore libjsoncpp.so libuv.so pthread)
#install
install(TARGETS ${TARGET_APP} DESTINATION ${INSTALL_APP_BIN})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})
#install(DIRECTORY configs/ DESTINATION ${INSTALL_DIR}/../configs/net)

