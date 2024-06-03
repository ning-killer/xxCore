cmake_minimum_required(VERSION 3.2)

project(demo)

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(PLATFORM ck804)
set(CPU_FLAGS "-mcpu=ck804f -mfpu=auto -mfloat-abi=hard -mlittle-endian")
set(LINK_SCRIPT ${ROOT_PATH}/libs/32bit/gcc_csky.ld)
set(CMAKE_C_COMPILER csky-abiv2-elf-gcc)
set(CMAKE_CXX_COMPILER csky-abiv2-elf-g++)
#set(CMAKE_LINKER csky-abiv2-elf-g++)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

add_definitions(-DCK804)
add_link_options(-nostartfiles -Wl,-T${LINK_SCRIPT} -Wl,-gc-sections -Wl,-zmax-page-size=1024 -Wl,-Map=cardetect.map)


if(CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Release")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CPU_FLAGS} -Wall -std=c99 -Os -pipe -Werror -Wno-unused-variable")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CPU_FLAGS} -Wall -std=c++11 -Os -pipe -Werror -Wno-unused-variable")
else()
	set(CMAKE_BUILD_TYPE "Debug")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  ${CPU_FLAGS} -Wall -std=c99 -c -fmessage-length=0 -O0 -g -pipe -Werror -Wno-unused-variable")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  ${CPU_FLAGS} -Wall -std=c++11 -c -fmessage-length=0 -O0 -g -pipe -Werror -Wno-unused-variable")
endif()

set(INC_DIRS ${ROOT_PATH}/include ${ROOT_PATH}/code/build/export/include)
set(LIB_DIRS ${ROOT_PATH}/libs/32bit ${ROOT_PATH}/code/build/export)
set(BUILD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build_${PLATFORM}/${CMAKE_BUILD_TYPE})

set(LL_LINK_LIBS rne_rt_g2 rne_pal_${PLATFORM} ${PLATFORM}_bsp)
set(START_GRP "-Wl,--start-group")
set(END_GRP "-Wl,--end-group")


aux_source_directory(./src DIR_SRC)

set(target output_crc_demo.elf)

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
