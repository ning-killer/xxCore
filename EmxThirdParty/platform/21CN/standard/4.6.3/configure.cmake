#编译依赖
set(21CN_Standard_Version 4.6.3)

#libs
set(21CN_Standard_LIBS
        # libcjson.a
        libcommon.a
        libcos.a
        libthirdparty.a
        libCAudioCloud.a
        )

#inc_dir
set(21CN_Standard_INC_DIR
        include
        )

#lib_dir
#外部链接指定工具链
set(21CN_Standard_LIB_DIR
        lib
        )

#definitions
add_definitions(
        -DXXCN_Standard_Version="${21CN_Standard_Version}"
)