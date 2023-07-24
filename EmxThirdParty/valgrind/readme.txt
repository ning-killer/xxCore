# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# valgrind-3.21.0

修改configure文件，将
     armv7*)
	{ printf "%s\n" "$as_me:${as_lineno-$LINENO}: result: ok (${host_cpu})" >&5
printf "%s\n" "ok (${host_cpu})" >&6; }
	ARCH_MAX="arm"
	;;
更改为
     arm*)
	{ printf "%s\n" "$as_me:${as_lineno-$LINENO}: result: ok (${host_cpu})" >&5
printf "%s\n" "ok (${host_cpu})" >&6; }
	ARCH_MAX="arm"
	;;
也就是去掉两个字符v7

./configure --prefix=${PWD}/out/arm-cvitek-linux-uclibcgnueabihf --host=arm-cvitek-linux-uclibcgnueabihf

make -j32
# 安装，最终文件会安装到当前的out目录下
make install
# 将include和lib放入EmxThirdParty中的相应目录下


编译需要检测的应用程序时需要保证应用程序和相关库打开-g并且不要strip，执行valgrind之前需要执行例如
export VALGRIND_LIB=/mnt/nfs/MemAnalyze/out/libexec/valgrind，使得valgrind可以找到自己依赖的动态库文件

