# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# cJSON-1.7.15
mkdir build

cd build

cmake -DENABLE_CJSON_TEST=OFF -DBUILD_SHARED_AND_STATIC_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PWD}/out/arm-cvitek-linux-uclibcgnueabihf -DCMAKE_C_COMPILER=/opt/cvitek/arm-cvitek-linux-uclibcgnueabihf/bin/arm-cvitek-linux-uclibcgnueabihf-gcc ..

make -j32
# 安装，最终文件会安装到当前的out目录下
make install
# 将include和lib放入EmxThirdParty中的相应目录下


