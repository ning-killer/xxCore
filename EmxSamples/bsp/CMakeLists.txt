cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE apps RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

foreach(file IN LISTS apps)
    #src
    get_filename_component(filename ${file} NAME_WE)
    add_executable(${filename} ${file})
    strip(${filename} ${filename})

    #include
    target_include_directories(${filename} PRIVATE ${BspPath}/include)

    #lib
    target_link_libraries(${filename}
        Bsp
        EmxCore  
        libjsoncpp.so 
        libuv.so 
        pthread
    )

    #install
    install(TARGETS ${filename} DESTINATION ${INSTALL_APP_BIN})
endforeach()

