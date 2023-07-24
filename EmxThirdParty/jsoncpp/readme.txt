# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# jsoncpp-1.9.3
mkdir build

cd build

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_LIBDIR=${PWD}/out/lib -DCMAKE_INSTALL_INCLUDEDIR=${PWD}/out/include -DJSONCPP_WITH_TESTS=OFF -DCMAKE_CXX_COMPILER=/opt/cvitek/arm-cvitek-linux-uclibcgnueabihf/bin/arm-cvitek-linux-uclibcgnueabihf-g++ ..

make -j32
# 安装，最终文件会安装到当前的out目录下
make install
# 将include和lib放入EmxThirdParty中的相应目录下


