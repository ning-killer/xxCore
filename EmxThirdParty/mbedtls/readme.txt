# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# 连接顺序`mbedtls mbedx509 mbedcrypto`
# -----------------------------------------------------------------------------
# mbedtls-2.16.9
# 建立编译目录
mkdir -p build
cd build
rm -r *
# 修改CMakeList.txt
# 注释掉如下几行，仅保留option(ENABLE_TESTING "Build mbed TLS tests." OFF)
if(CMAKE_COMPILER_IS_MSVC)
    option(ENABLE_TESTING "Build mbed TLS tests." OFF)
else()
    option(ENABLE_TESTING "Build mbed TLS tests." ON)
endif()

# 将ENABLE_PROGRAMS配置为OFF，option(ENABLE_PROGRAMS "Build mbed TLS programs." OFF)


# 配置，注意更换你所希望使用的交叉编译链
cmake -DCMAKE_C_COMPILER=arm-cvitek-linux-uclibcgnueabihf-gcc -DCMAKE_BUILD_TYPE=Release -DUSE_SHARED_MBEDTLS_LIBRARY=On -DCMAKE_INSTALL_PREFIX=${PWD}/out ..

# 编译
make -j32
# 安装，最终文件会安装到当前的build/out目录下
make install
# 将include和lib放入EmxThirdParty中的相应目录下


