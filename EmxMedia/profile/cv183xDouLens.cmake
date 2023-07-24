set(HARDWARE_PLATFORM cvitek)
set(CHIP cv183x)
set(CROSS aarch64-linux-gnu)

set(SDK_VERSION 2.2.0.1)
add_compile_options(
        -DSENSOR_OV_OS04C10
        -DSENSOR_OV_OS04C10_SLAVE
        -DSUPPORT_DOUBLE_SENSOR_ISP
        #-DSUPPORT_ISP_PQTOOL
        -DSV_VS3D6MN
)