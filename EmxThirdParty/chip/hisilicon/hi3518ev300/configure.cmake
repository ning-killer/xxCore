add_compile_options(
        -Wall -g -lpthread -lm -ldl
        -DHICHIP=0x3516E200
        -DHI_RELEASE -DHI_XXXX
        -DHI_ACODEC_TYPE_INNER
        -DISP_V2
        -mcpu=cortex-a7
        -mno-unaligned-access
        -fno-aggressive-loop-optimizations
        -ffunction-sections
        -fdata-sections
        -fstack-protector
)
add_link_options(-fPIC)
set(MEDIA_LIBS
        libmpi.a
        libmd.a
        libVoiceEngine.a
        libupvqe.a
        libdnvqe.a
        lib_hiae.a
        libisp.a
        lib_hiae.a
        lib_hiawb.a
        lib_hidrc.a
        lib_hildci.a
        lib_hidehaze.a
        libbcd.a
        libivp.a
        libive.a
        libsecurec.so
        )

