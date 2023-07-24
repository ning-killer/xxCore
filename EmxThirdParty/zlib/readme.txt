# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# zlib-1.2.11

mkdir build

cd build

cmake -DCMAKE_BUILD_TYPE=Release \
-DINSTALL_BIN_DIR=${PWD}/out/bin \
-DINSTALL_LIB_DIR=${PWD}/out/lib \
-DINSTALL_INC_DIR=${PWD}/out/include \
-DINSTALL_MAN_DIR=${PWD}/out/shared \
-DINSTALL_PKGCONFIG_DIR=${PWD}/out/pkg \
-DCMAKE_C_COMPILER=/opt/cvitek/arm-cvitek-linux-uclibcgnueabihf/bin/arm-cvitek-linux-uclibcgnueabihf-gcc \
..

make -j32
# 安装，最终文件会安装到当前的out目录下
make install
# 将include和lib放入EmxThirdParty中的相应目录下


