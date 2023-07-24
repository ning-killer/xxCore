#编译依赖
set(AndLinkVersion 1.5.0)

#libs
#需要保证各迭代版本不会存在因工具链差异导致依赖库差异
set(ANDLINK_LIBS
        andlink.a
        )

#inc_dir
set(ANDLINK_INC_DIR
        include
        )

#lib_dir
#外部链接指定工具链
set(ANDLINK_LIB_DIR
        lib
        )

#definitions
add_definitions(
        -DAndLinkVersion="${AndLinkVersion}"
)