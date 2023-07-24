
set(EXE_LIB
        pjsip-x86_64-unknown-linux-gnu uuid
        pj-x86_64-unknown-linux-gnu
        pjlib-util-x86_64-unknown-linux-gnu
        pthread z
        spdlog
        m)

if(svs)
  list(APPEND EXE_LIB svscc)
  
  include_directories( 
  ${THIRDPARTY_PATH}/svs/include)

  link_directories(
  ${THIRDPARTY_PATH}/svs/lib)
endif()

aux_source_directory(${PROJECT_SOURCE_DIR} SRCS)

include_directories(
        ${PROJECT_SOURCE_DIR}
       
        ${THIRDPARTY_PATH}/pjsip/include
		${THIRDPARTY_PATH}/spdlog/include)

link_directories(
        ${THIRDPARTY_PATH}/pjsip/lib
		${THIRDPARTY_PATH}/spdlog/lib)
