#  -DSENSOR0_TYPE=SONY_IMX334_MIPI_8M_30FPS_12BIT
#  -DSENSOR1_TYPE=SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT
#  -DSENSOR2_TYPE=SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT
#  -DSENSOR3_TYPE=SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT
#  -DSENSOR4_TYPE=SONY_IMX334_MIPI_8M_30FPS_12BIT
add_compile_options(
        -Wall -O2 -g -lpthread -lm -ldl
        -Dhi3519av100
        -DHI_XXXX
        -DHI_RELEASE
        -DHI_ACODEC_TYPE_INNER
        -DHI_ACODEC_TYPE_HDMI
        -DISP_V2
        -DVER_X=1
        -DVER_Y=0
        -DVER_Z=0
        -DVER_P=0
        -DVER_B=10
        -DUSER_BIT_32
        -DKERNEL_BIT_32
        -mcpu=cortex-a53
        -mfloat-abi=softfp
        -mfpu=neon-vfpv4
        -fno-aggressive-loop-optimizations
        -ffunction-sections
        -fdata-sections
        -fstack-protector-strong
        -Wno-error=implicit-function-declaration
        -Wno-date-time
)
add_link_options(-fPIC)
set(MEDIA_LIBS
        libmpi.a
        libive.a
        libmd.a
        libnnie.a
        libdsp.a
        libhdmi.a
        libdsp.a
        libmotionfusion.a
        lib_hiae.a
        libisp.a
        lib_hidehaze.a
        lib_hidrc.a
        lib_hildci.a
        lib_hicalcflicker.a
        lib_hiawb.a
        libsns_imx290.a
        libsns_imx290_slave.a
        libsns_imx334.a
        libVoiceEngine.a
        libupvqe.a
        libdnvqe.a
        libaacenc.a
        libaacdec.a
        libsecurec.a
        )

