# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
--enable-threaded-resolver选项很重要，未添加此选项，出现路由器断网，设备重复调用curl_easy_perform，程序报Bus error导致设备重启问题

# -----------------------------------------------------------------------------
# 如果是curl-7.72.0-mbedtls则使用下面命令
./configure --prefix=${PWD}/out --host=arm-cvitek-linux-uclibcgnueabihf --enable-threaded-resolver --with-mbedtls=/home/xiong/workspace/test/3dr/EmxThirdParty/mbedtls/2.4.2/arm-cvitek-linux-uclibcgnueabihf --without-ssl
make CCLD="arm-cvitek-linux-uclibcgnueabihf-gcc -lmbedcrypto -lmbedtls -lmbedx509" -j32

# 如果是curl-7.72.0-openssl则使用下面命令
./configure --prefix=${PWD}/out --host=arm-cvitek-linux-uclibcgnueabihf --enable-threaded-resolver --with-ssl=/home/xiong/workspace/test/3dr/EmxThirdParty/openssl/1.0.2t/arm-cvitek-linux-uclibcgnueabihf --without-mbedtls
make CCLD="arm-cvitek-linux-uclibcgnueabihf-gcc -lcrypto -lssl" -j32


# 安装，最终文件会安装到当前的out目录下
make install
# 将include,bin和lib放入EmxThirdParty中的相应目录下


