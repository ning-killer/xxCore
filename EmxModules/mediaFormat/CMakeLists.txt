cmake_minimum_required(VERSION 3.10)

include_directories(include)
add_definitions(-DMOV_READER_FMP4_FAST)

##lib flv
#aux_source_directory(lib/flv SRCS_FLV)
#aux_source_directory(lib/flv/libflv/source SRCS_FLV)
#add_library(Flv SHARED ${SRCS_FLV})
#strip(Flv libFlv.so)
#
#target_include_directories(Flv PRIVATE lib/flv/libflv/include)
#target_compile_options(Flv PRIVATE -Wno-unused-but-set-variable)
#target_compile_options(Flv PRIVATE -Wno-unused-value)
#target_compile_options(Flv PRIVATE -Wno-maybe-uninitialized)
#target_compile_options(Flv PRIVATE -Wno-unused-function)
#install(TARGETS Flv DESTINATION ${INSTALL_APP_LIB})
#
#lib mp4
aux_source_directory(lib/mp4 SRCS_MP4)
aux_source_directory(lib/mp4/libmov/source SRCS_MP4)
add_library(Mp4 SHARED ${SRCS_MP4})
strip(Mp4 libMp4.so)
target_include_directories(Mp4 PRIVATE lib/mp4/libmov/include)
target_compile_options(Mp4 PRIVATE -Wno-unused-but-set-variable)
install(TARGETS Mp4 DESTINATION ${INSTALL_APP_LIB})

##lib mkv
#aux_source_directory(lib/mkv SRCS_MKV)
#aux_source_directory(lib/mkv/libmkv/source SRCS_MKV)
#add_library(Mkv SHARED ${SRCS_MKV})
#strip(Mkv libMkv.so)
#target_include_directories(Mkv PRIVATE lib/mkv/libmkv/include)
#target_compile_options(Mkv PRIVATE -Wno-unused-but-set-variable)
#install(TARGETS Mkv DESTINATION ${INSTALL_APP_LIB})

#lib rtp
aux_source_directory(lib/rtp SRCS_MKV)
aux_source_directory(lib/rtp/librtp/source SRCS_MKV)
aux_source_directory(lib/rtp/librtp/payload SRCS_MKV)
add_library(Rtp SHARED ${SRCS_MKV})
strip(Rtp libRtp.so)
target_include_directories(Rtp PRIVATE lib/rtp/librtp/include)
target_compile_options(Rtp PRIVATE -Wno-unused-but-set-variable)
target_compile_options(Rtp PRIVATE -Wno-unused-function)
install(TARGETS Rtp DESTINATION ${INSTALL_APP_LIB})

#install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE})

##test fmt
#aux_source_directory(test SRCS_test)
#add_executable(fmt ${SRCS_test})
#target_link_libraries(fmt Flv Mp4 SDCard EmxMedia EmxCore libjsoncpp.so libuv.so pthread)
#install(TARGETS fmt DESTINATION ${INSTALL_APP_BIN})
#strip(fmt fmt)
