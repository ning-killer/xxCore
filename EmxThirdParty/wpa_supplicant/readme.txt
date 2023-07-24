# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# wpa_supplicant-2.9&libnl-3.2.25
# 首先编译libnl静态库,注意只保留静态库，不要动态库
./configure --prefix=${PWD}/out/arm-cvitek-linux-uclibcgnueabihf --host=arm-cvitek-linux-uclibcgnueabihf --disable-shared
make -j32
make install

# 然后编译wpa应用程序
make clean
# 拷贝2.9/emx_config配置文件到wpa_supplicant-2.9/wpa_supplicant目录中命名为.config
cp emx_config .config
# 修改.config文件
# 找到如下两行，将依赖libnl库改为上面编译后的安装路径
CFLAGS += -I/home/xiong/workspace/test/3dr/EmxThirdParty/source/libnl-3.2.25/out/arm-cvitek-linux-uclibcgnueabihf/include/libnl3
LIBS += -L/home/xiong/workspace/test/3dr/EmxThirdParty/source/libnl-3.2.25/out/arm-cvitek-linux-uclibcgnueabihf/lib -lpthread
# 然后编译安装
make CC=arm-cvitek-linux-uclibcgnueabihf-gcc -j32
make install


