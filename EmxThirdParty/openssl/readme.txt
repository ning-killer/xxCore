# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# openssl-1.0.2t
# 清空临时文件
make distclean
# 配置，注意更换你所希望使用的交叉编译链
./config no-asm shared no-async --prefix=${PWD}/out --cross-compile-prefix=arm-cvitek-linux-uclibcgnueabihf-
# 删除MakeFile中的-m64
sed -i 's/-m64//g' Makefile
# 编译
make -j32
# 安装，最终文件会安装到当前的out目录下
make install
# 删掉除lib和include外的其他目录，lib中仅保留libssl.*和libcrypto.*
# 将include和lib放入EmxThirdParty中的相应目录下
