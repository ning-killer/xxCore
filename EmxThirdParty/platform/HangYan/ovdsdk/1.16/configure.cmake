#编译依赖
set(OvdVersion 1.16)

#libs
#需要保证各迭代版本不会存在因工具链差异导致依赖库差异
#链接mbedtls库的顺序为-lmbedtls -lmbedx509 -lmbedcrypto, 顺序不可改变。
set(OVDSDK_LIBS
        libovdsdk.a
        libcjson.a
        libmbedtls.a
        libmbedx509.a
        libmbedcrypto.a
        )

#inc_dir
set(OVDSDK_INC_DIR
        include
        include/cjson
        # 自行选择是否外部依赖
        # include/mbedtls
        # include/curl
        )

#lib_dir
#外部链接指定工具链
set(OVDSDK_LIB_DIR
        lib
        )

#definitions
add_definitions(
        #软探针版本说明：https://docs.qq.com/doc/DVUZjQ3hZaldGS0pC
        #针对OVD版本迭代导致API发生变化的符号定义
        #DOVDSDK_APIVER_num1_num2定义说明: num1(针对sdk升级带来的(大差异)软探针版本升级：该参数为platform提供说明)，num2(针对sdk升级带来的api、struct发生差异：该参数为apil自行维护)
        -DOVDSDK_APIVER_1_0 
        -DOvdVersion="${OvdVersion}"
        -DCONFIG_MODULE_softprobe
)