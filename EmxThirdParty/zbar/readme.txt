# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# zbar-0.10
# 替换zbar源码目录config下面的config.guess和config.sub这两个文件，用于解决aarch64无法编译问题
./configure --prefix=${PWD}/out/arm-cvitek-linux-uclibcgnueabihf --host=arm-cvitek-linux-uclibcgnueabihf --disable-video --without-xshm --without-xv --without-jpeg --without-imagemagick --without-gtk --without-python --without-qt --with-x=no CFLAGS="-DNDEBUG"

make -j32
# 安装，最终文件会安装到当前的out目录下
make install
# 将include和lib放入EmxThirdParty中的相应目录下


