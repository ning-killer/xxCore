libvoipengine_not_include_mbedtls.a是不包含mbedtls库的SDK,

固话安防融合编译版本, mbedtls等公共库在common/lib目录下, 链接mbedtls库的顺序为-lmbedtls -lmbedx509 -lmbedcrypto, 顺序不可改变。

库编译的时候添加了编译参数 -ffunction-sections -fdata-sections, 编译进程的时候添加 -Wl,--gc-sections 不链接未使用的代码段和数据段, 可减少进程体积和内存使用。
