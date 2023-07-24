# 编译说明
# -----------------------------------------------------------------------------
# gdb-9.2
mkdir build

cd build

../configure --prefix=/home/hn/eapil-work/3rd_lib/gdb-9.2/bin/opt/gdb-9.2 --host=arm-cvitek-linux-uclibcgnueabihf LD=arm-cvitek-linux-uclibcgnueabihf-ld CC=arm-cvitek-linux-uclibcgnueabihf-gcc CXX=arm-cvitek-linux-uclibcgnueabihf-g++

make -j6 && make install


