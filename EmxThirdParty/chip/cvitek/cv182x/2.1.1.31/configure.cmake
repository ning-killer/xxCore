add_link_options(-fPIC)
set(MEDIA_LIBS
        libcvi_bin.a
        libcvi_bin_isp.a
        libz.a
        #video
        libcvi_ispd.so
        libisp.a
        libisp_algo.a
        libae.a
        libawb.a
        libaf.a
        libsns_full.a
        

        #encode
        libvenc.a
        #libvpu.a
        vpu
        libcvi_vcodec.a
        libcvi_jpeg.a
        libcviai_app-static.a
        #libcvitracer.a
        cvitracer

        #audio
        libcvi_audio.a
        libcli.a
        libcvi_vqe.a
        cvi_ssp
        libcvi_VoiceEngine.a
        libcvi_RES1.a
        libdnvqe.a
        libtinyalsa.a
        cvi_ssp
        
        #sys
        libini.a
        #libsys.a
        sys
        #AI
        cviai_core
        cviai_service
        cviruntime
        cnpy
        cvimath
        cvikernel
        cvi_ive_tpu

        #        libcviai_core-static.a
        #        libcviai_service-static.a
        #        libcviruntime-static.a
        #        libcvikernel-static.a
        #        libcnpy.a
        #        liblz4.a
        #        libcvi_ive_tpu-static.a
        #        libcvimath-static.a
        #        #opencv
        #        libopencv_imgproc.a
        #        libopencv_core.a
        #        libtegra_hal.a
        #        boost_system
        )
set(MEDIA_SRC
        sample
        )
set(MEDIA_LIB_DIR
        lib
        )
set(MEDIA_INC_DIR
        sample
        include
        include/isp/cv182x
        include/ive
        include/linux
        include/glog
        include/cviai
        )
add_definitions(
        #-DCV1821
        #-DCV1821_WATCHDOG
        #-DCV1821_ADC
        #-DSUPPORT_EXTERNAL_AAC
        -DCVI_MODIFIED
        #-DCV1821_ROTATE_180
        #-DPERSON_REGION_SUPPORT
        #-DICR_AE_SUPPORT
        #-DCV1821_OSD_OVERLAYEX_SUPPORT
        #-DSUPPORT_ISP_PQTOOL
)
