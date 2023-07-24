# 注意：所有第三方库编译的时候不要进行strip，strip操作由产品集成人员release的时候进行
# 编译说明
# -----------------------------------------------------------------------------
# pjsip版本号未知
./configure --disable-resample --disable-pjsua2 --disable-sound --disable-video --disable-sdl --disable-ffmpeg --disable-vpx --disable-ipp --disable-ssl --disable-small-filter --disable-large-filter --disable-speex-aec --disable-g711-codec --disable-l16-codec --disable-gsm-codec --disable-g722-codec --disable-g7221-codec --disable-speex-codec  --disable-ilbc-codec --disable-srtp --disable-libyuv --disable-libwebrtc --prefix=$(pwd)/out/arm-cvitek-linux-uclibcgnueabihf --host=arm-cvitek-linux-uclibcgnueabihf CFLGAS='-fPIC'
make -j32
make install

#最终安装文件仅须保留
├── include
│   ├── pj
│   ├── pjlib.h
│   ├── pjlib-util
│   ├── pjlib-util.h
│   ├── pjsip
│   ├── pjsip_auth.h
│   └── pjsip.h
└── lib
    ├── libpj-arm-cvitek-linux-uclibcgnueabihf.a
    ├── libpjlib-util-arm-cvitek-linux-uclibcgnueabihf.a
    └── libpjsip-arm-cvitek-linux-uclibcgnueabihf.a

