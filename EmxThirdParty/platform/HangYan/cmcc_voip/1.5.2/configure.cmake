#编译依赖
set(VoipVersion 1.5.2)
set(VoipApiVersion 1.5.x)

#libs
#需要保证各迭代版本不会存在因工具链差异导致依赖库差异
set(VOIP_LIBS
        voipengine.a
        )

#inc_dir
set(VOIP_INC_DIR
        include
        )

#lib_dir
#外部链接指定工具链
set(VOIP_LIB_DIR
        lib
        )

#definitions
add_definitions(
        -DVoipVersion="${VoipVersion}"
        -DVoipApiVersion="${VoipApiVersion}"
)