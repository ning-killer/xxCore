cmake_minimum_required(VERSION 3.2)

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

set(INC_DIRS ${ROOT_PATH}/include)
set(LIB_DIRS ${ROOT_PATH}/libs/32bit)
set(BUILD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build_${PLATFORM}/${CMAKE_BUILD_TYPE})

set(LL_LINK_LIBS rne_pal_linux_a7 rne_rt_g3 c m pthread)
set(START_GRP "-Wl,--start-group")
set(END_GRP "-Wl,--end-group")


aux_source_directory(./src DIR_SRC)

set(target custom_operator_layer_demo.elf)

include_directories(${INC_DIRS})
link_directories(${LIB_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${BUILD_PATH})

add_executable(${target} ${DIR_SRC})

target_link_libraries(
${target}
${START_GRP}
${LL_LINK_LIBS}
${END_GRP}
)
