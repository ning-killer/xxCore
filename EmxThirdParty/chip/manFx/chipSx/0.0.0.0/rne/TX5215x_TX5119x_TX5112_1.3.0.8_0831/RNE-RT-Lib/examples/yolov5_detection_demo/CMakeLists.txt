cmake_minimum_required(VERSION 3.13)

project(demo)

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(PLATFORM linux_a7)
set(CPU_FLAGS "-mcpu=cortex-a7 -mfpu=vfp -mfloat-abi=hard")
set(CMAKE_C_COMPILER arm-none-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-none-linux-gnueabihf-g++)
#set(CMAKE_LINKER arm-none-linux-gnueabihf-g++)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")


if(CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Release")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CPU_FLAGS} -Wall -std=c99 -O2 -fPIC -ffunction-sections -fdata-sections")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CPU_FLAGS} -Wall -std=c++11 -O2 -fPIC -ffunction-sections -fdata-sections")
else()
	set(CMAKE_BUILD_TYPE "Debug")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  ${CPU_FLAGS} -Wall -std=c99 -c -fmessage-length=0 -O0 -g -fPIC -ffunction-sections -fdata-sections")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  ${CPU_FLAGS} -Wall -std=c++11 -c -fmessage-length=0 -O0 -g -fPIC -ffunction-sections -fdata-sections")
endif()

set(INC_DIRS include ${ROOT_PATH}/include ${ROOT_PATH}/opencv4.5.5/include/opencv4)
set(LIB_DIRS ${ROOT_PATH}/libs/64bit ${ROOT_PATH}/opencv4.5.5/libs  ${ROOT_PATH}/opencv4.5.5/libs/opencv4/3rdparty)
set(BUILD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build_${PLATFORM}/${CMAKE_BUILD_TYPE})

set(LL_LINK_LIBS rne_common rne_pal_linux_a53 rne_rt_g3 c m pthread opencv_world libtiff zlib libjpeg-turbo ade libopenjp2 libprotobuf libwebp quirc libpng dl)
set(START_GRP "-Wl,--start-group")
set(END_GRP "-Wl,--end-group")

set(OPENSOURCE_DIR  "../../open_source/record_file"
					"../../open_source/gp_layers")

set(SOURCE_DIR  "src"
				${OPENSOURCE_DIR})

set(ALL_C_FILES "")

foreach(DIR ${SOURCE_DIR})
	file(GLOB C_FILES "${DIR}/*.cpp" "${DIR}/*.c")
	list(APPEND ALL_C_FILES ${C_FILES})
endforeach()

get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(target ${CURRENT_DIR_NAME}.elf)

include_directories(${INC_DIRS} ${OPENSOURCE_DIR})
link_directories(${LIB_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${BUILD_PATH})


add_executable(${target} ${ALL_C_FILES})

target_link_libraries(
${target}
${START_GRP}
${LL_LINK_LIBS}
${END_GRP}
)
