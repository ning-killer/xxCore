cmake_minimum_required(VERSION 3.2)

project(demo)

add_link_options(-Wl,--gc-sections)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
#set(CMAKE_LINKER g++)

if(CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Release")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -m64 -Wall -std=c99 -O2 -Werror -fPIC -ffunction-sections -fdata-sections")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -m64 -Wall -std=c++11 -O2 -Werror -fPIC -ffunction-sections -fdata-sections")
else()
	set(CMAKE_BUILD_TYPE "Debug")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -m64 -Wall -std=c99 -c -fmessage-length=0 -O0 -g -Werror -fPIC -ffunction-sections -fdata-sections")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -m64 -Wall -std=c++11 -c -fmessage-length=0 -O0 -g -Werror -fPIC -ffunction-sections -fdata-sections")
endif()

set(INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
set(LIB_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/64bit)
set(BUILD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build_sim/${CMAKE_BUILD_TYPE})
#list(APPEND INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LL_LINK_LIBS rne_pal_sim rne_vm rne_rt_g3 c m pthread stdc++)
set(START_GRP "-Wl,--start-group")
set(END_GRP "-Wl,--end-group")


aux_source_directory(./src DIR_SRC)

set(target multi_net_forward_demo.elf)

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
