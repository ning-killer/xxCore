# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# libturbojpeg


cmake -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc -DCMAKE_C_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX=./_install
make && make install

# 将include和lib放入EmxThirdParty中的相应目录下


