#编译依赖
set(21CN_Cloud_Version 3.0.1)

# #libs
# #libmbedtls调整为外部依赖，需要保证版本统一为2.23.0
set(21CN_Cloud_LIBS
        libcloudcamera.a
        libsdccos.a
        libsdcthirdparty.a
        #libshframebuf.a
        #libmbedtls.a
        #libmbedx509.a
        #libmbedcrypto.a
        # libmi_sys.a
        # libcam_os_wrapper.a
        libz.a
        )

#inc_dir
set(21CN_Cloud_INC_DIR
        include
        )

#lib_dir
#外部链接指定工具链
set(21CN_Cloud_LIB_DIR
        lib
        )

#definitions
add_definitions(
        -DXXCN_Cloud_Version="${21CN_Cloud_Version}"
)