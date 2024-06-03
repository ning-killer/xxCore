#编译依赖
set(EpGbVersion 1.0.0)

#libs
#需要保证各迭代版本不会存在因工具链差异导致依赖库差异
set(EPGBSDK_LIBS
        libep-gbsdk.so
        )

#inc_dir
set(EPGBSDK_INC_DIR
        include
        )

#lib_dir
#外部链接指定工具链
set(EPGBSDK_LIB_DIR
        lib
        )

#definitions
add_definitions(
        -DEpGbVersion="${EpGbVersion}"
)