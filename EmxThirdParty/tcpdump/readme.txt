# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# tcpdump-4.99.0&libpcap-1.10.0

# 首先编译libpcap静态库,注意只保留静态库，不要动态库
./configure --prefix=${PWD}/out/arm-cvitek-linux-uclibcgnueabihf --host=arm-cvitek-linux-uclibcgnueabihf --disable-shared
make -j32
make install
# 然后编译tcpdump应用程序,链接刚刚生成的libpcap
./configure --prefix=${PWD}/out/arm-cvitek-linux-uclibcgnueabihf --host=arm-cvitek-linux-uclibcgnueabihf CFLAGS="-I/home/xiong/workspace/test/3dr/EmxThirdParty/source/libpcap-1.10.0/out/arm-cvitek-linux-uclibcgnueabihf/include -L/home/xiong/workspace/test/3dr/EmxThirdParty/source/libpcap-1.10.0/out/arm-cvitek-linux-uclibcgnueabihf/lib"
make -j32
make install


