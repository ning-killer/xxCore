# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# libconfig-1.7.3
mkdir build

cd build

../configure --prefix=${PWD}/out/arm-sigmastar-linux-uclibcgnueabihf --host=arm-sigmastar-linux-uclibcgnueabihf LD=arm-sigmastar-linux-uclibcgnueabihf-ld CC=arm-sigmastar-linux-uclibcgnueabihf-gcc CXX=arm-sigmastar-linux-uclibcgnueabihf-g++

make -j32
# 安装，最终文件会安装到当前的out目录下
make install
# 将include和lib放入EmxThirdParty中的相应目录下


