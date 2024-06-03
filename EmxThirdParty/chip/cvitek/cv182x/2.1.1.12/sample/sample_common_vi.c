/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample/common/sample_common_vi.c
 * Description:
 *   Common sample code for video input.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cvi_mipi.h"
#include "cvi_sns_ctrl.h"
#include "cvi_common.h"
#include "cvi_awb_comm.h"
#include "cvi_af_comm.h"
#include "cvi_comm_isp.h"
#include "sample_comm.h"
#include "cvi_isp.h"
#include "ini.h"

#define SNSCFGPATH_SIZE 100
static CVI_CHAR g_snsCfgPath[SNSCFGPATH_SIZE];

VI_DEV_ATTR_S DEV_ATTR_NEXTCHIP_N5_2M_BASE = {
	VI_MODE_BT656,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,//VI_SCAN_INTERLACED,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_UYVY,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_PIXELPLUS_PR2020_1M_BASE = {
	VI_MODE_BT656,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,//VI_SCAN_INTERLACED,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_UYVY,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1280,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            720,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1280, 720},
	{
		WDR_MODE_NONE,
		720
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_PIXELPLUS_PR2020_2M_BASE = {
	VI_MODE_BT656,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,//VI_SCAN_INTERLACED,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_UYVY,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_PIXELPLUS_PR2100_2M_BASE = {
	VI_MODE_MIPI_YUV422,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,//VI_SCAN_INTERLACED,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_UYVY,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_PIXELPLUS_PR2100_2M_2CH_BASE = {
	VI_MODE_MIPI_YUV422,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,//VI_SCAN_INTERLACED,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_UYVY,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            3844,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1124,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{3844, 1124},
	{
		WDR_MODE_NONE,
		1124
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_PIXELPLUS_PR2100_2M_4CH_BASE = {
	VI_MODE_MIPI_YUV422,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,//VI_SCAN_INTERLACED,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_UYVY,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            7688,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1124,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{7688, 1124},
	{
		WDR_MODE_NONE,
		1124
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_OV4689_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2688,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	0,            1520,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2688, 1520},
	{
		WDR_MODE_NONE,
		1520
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_OS08A20_8M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            3840,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            2160,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{3840, 2160},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_OS08A20_5M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2592,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1944,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2592, 1944},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_OS08A20_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2560,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1440,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

#ifdef FPGA_PORTING
VI_DEV_ATTR_S DEV_ATTR_IMX327_1M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1280,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            720,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1280, 720},
	{
		WDR_MODE_NONE,
		720
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};
#endif

VI_DEV_ATTR_S DEV_ATTR_IMX327_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX307_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX334_8M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            3840,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            2160,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{3840, 2160},
	{
		WDR_MODE_NONE,
		2160
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX335_5M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2592,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1944,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2592, 1944},
	{
		WDR_MODE_NONE,
		1944
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};
VI_DEV_ATTR_S DEV_ATTR_IMX335_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2560,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1440,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};
VI_DEV_ATTR_S DEV_ATTR_IMX335_4M_1600P_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2560,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1600,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2560, 1600},
	{
		WDR_MODE_NONE,
		1600
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};
VI_DEV_ATTR_S DEV_ATTR_IMX335_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};
VI_DEV_ATTR_S DEV_ATTR_IMX385_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync	    port_hsync_neg	  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,	       1920,	    0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,	       1080,	    0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,	       0,	     0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};
VI_DEV_ATTR_S DEV_ATTR_F23_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,           0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_F35_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_H65_1M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1280,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            720,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1280, 720},
	{
		WDR_MODE_NONE,
		720
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_K04_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2688,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1520,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2688, 1520},
	{
		WDR_MODE_NONE,
		1520
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX297_1M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1280,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            720,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1280, 720},
	{
		WDR_MODE_2To1_LINE,
		720
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX297_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1280,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            720,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_VIVO_MCS369Q_4M_BASE = {
	VI_MODE_BT1120_STANDARD,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2560,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1440,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_VIVO_MCS369_2M_BASE = {
	VI_MODE_BT1120_STANDARD,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_VIVO_MM308M2_2M_BASE = {
	VI_MODE_BT1120_STANDARD,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_PICO_640_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            632,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            479,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{632, 479},
	{
		WDR_MODE_2To1_LINE,
		479
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_PICO_384_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            384,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            288,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{384, 288},
	{
		WDR_MODE_2To1_LINE,
		288
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX327_SUBLVDS_2M_BASE = {
	VI_MODE_LVDS,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX307_SUBLVDS_2M_BASE = {
	VI_MODE_LVDS,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_SC3335_3M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   2304,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1296,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{2304, 1296},
	{
		WDR_MODE_NONE,
		1296
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC3336_3M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   2304,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1296,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{2304, 1296},
	{
		WDR_MODE_NONE,
		1296
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC2335_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   1920,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1080,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC2336_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   1920,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1080,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC4336_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   2560,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1440,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC200AI_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   1920,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1080,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC500AI_5M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   2880,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1620,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{2880, 1620},
	{
		WDR_MODE_NONE,
		1620
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC500AI_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   2560,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1440,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC501AI_2L_5M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   2880,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1620,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{2880, 1620},
	{
		WDR_MODE_NONE,
		1620
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC401AI_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg	  port_hsync		port_hsync_neg		  */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act	hsync_hhb*/
	{0,		   2560,		0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,		   1440,		0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,		   0,			 0}
	},
	VI_DATA_TYPE_RGB,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC8238_8M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            3840,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            2160,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{3840, 2160},
	{
		WDR_MODE_NONE,
		2160
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC4210_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2560,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1440,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_NEXTCHIP_N6_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,//VI_SCAN_INTERLACED,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_UYVY,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_NEXTCHIP_N5_1M_BASE = {
	VI_MODE_BT656,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,//VI_SCAN_INTERLACED,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_UYVY,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1280,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            720,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_YUV,
	{1280, 720},
	{
		WDR_MODE_NONE,
		720
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_GC2053_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_GC2093_2M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_GC4653_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2560,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1440,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2560, 1440},
	{
		WDR_MODE_NONE,
		1440
	},
	.enBayerFormat = BAYER_FORMAT_GR,
};

VI_DEV_ATTR_S DEV_ATTR_GC1054_1M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1280,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            720,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1280, 720},
	{
		WDR_MODE_NONE,
		720
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_IMX347_4M_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            2688,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1520,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{2688, 1520},
	{
		WDR_MODE_NONE,
		1520
	},
	.enBayerFormat = BAYER_FORMAT_RG,
};

VI_DEV_ATTR_S DEV_ATTR_SC035HGS_480P_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            640,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            480,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{640, 480},
	{
		WDR_MODE_NONE,
		480
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_DEV_ATTR_S DEV_ATTR_SC035GS_480P_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            640,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            480,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{640, 480},
	{
		WDR_MODE_NONE,
		480
	},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_PIPE_ATTR_S PIPE_ATTR_1280x720_RAW12_420_3DNR_RFR = {
	VI_PIPE_BYPASS_NONE, CVI_FALSE, CVI_FALSE,
	1280, 720,
	PIXEL_FORMAT_RGB_BAYER_12BPP,
	COMPRESS_MODE_NONE,
	DATA_BITWIDTH_12,
	CVI_FALSE,
	CVI_FALSE,
	{ -1, -1},
	CVI_FALSE,
	CVI_FALSE
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR = {
	VI_PIPE_BYPASS_NONE, CVI_FALSE, CVI_FALSE,
	1920, 1080,
	PIXEL_FORMAT_RGB_BAYER_12BPP,
	COMPRESS_MODE_NONE,
	DATA_BITWIDTH_12,
	CVI_FALSE,
	CVI_FALSE,
	{ -1, -1},
	CVI_FALSE,
	CVI_FALSE
};

VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR = {
	VI_PIPE_BYPASS_NONE, CVI_FALSE, CVI_FALSE,
	3840, 2160,
	PIXEL_FORMAT_RGB_BAYER_12BPP,
	COMPRESS_MODE_NONE,
	DATA_BITWIDTH_10,
	CVI_FALSE,
	CVI_FALSE,
	{ -1, -1},
	CVI_FALSE,
	CVI_FALSE
};

VI_PIPE_ATTR_S PIPE_ATTR_2592x1944_RAW10_420_3DNR_RFR = {
	VI_PIPE_BYPASS_NONE, CVI_FALSE, CVI_FALSE,
	2592, 1944,
	PIXEL_FORMAT_RGB_BAYER_12BPP,
	COMPRESS_MODE_NONE,
	DATA_BITWIDTH_10,
	CVI_FALSE,
	CVI_FALSE,
	{ -1, -1},
	CVI_FALSE,
	CVI_FALSE
};

VI_CHN_ATTR_S CHN_ATTR_384x288_422_SDR8_LINEAR = {
	{384, 288},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	1,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_632x479_422_SDR8_LINEAR = {
	{632, 479},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	1,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1280x720_420_SDR8_LINEAR = {
	{1280, 720},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1280x720_422_SDR8_LINEAR = {
	{1280, 720},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SDR8_LINEAR = {
	{1920, 1080},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1920x1080_422_SDR8_LINEAR = {
	{1920, 1080},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2560x1440_422_SDR8_LINEAR = {
	{2560, 1440},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2560x1440_420_SDR8_LINEAR = {
	{2560, 1440},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2560x1600_420_SDR8_LINEAR = {
	{2560, 1600},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2592x1944_420_SDR8_LINEAR = {
	{2592, 1944},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2688x1520_420_SDR8_LINEAR = {
	{2688, 1520},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2880x1620_420_SDR8_LINEAR = {
	{2880, 1620},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_3840x2160_420_SDR8_LINEAR = {
	{3840, 2160},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_3844x1124_422_SDR8_LINEAR = {
	{3844, 1124},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_7688x1124_422_SDR8_LINEAR = {
	{7688, 1124},
	PIXEL_FORMAT_YUV_PLANAR_422,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_2304x1296_420_SDR8_LINEAR = {
	{2304, 1296},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_640x480_420_SDR8_LINEAR = {
	{640, 480},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1}
};

/*
 * Brief: get picture size(w*h), according enPicSize
 */
CVI_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S *pstSize)
{
	switch (enPicSize) {
	case PIC_CIF:   /* 352 * 288 */
		pstSize->u32Width  = 352;
		pstSize->u32Height = 288;
		break;

	case PIC_D1_PAL:   /* 720 * 576 */
		pstSize->u32Width  = 720;
		pstSize->u32Height = 576;
		break;

	case PIC_D1_NTSC:   /* 720 * 480 */
		pstSize->u32Width  = 720;
		pstSize->u32Height = 480;
		break;

	case PIC_720P:   /* 1280 * 720 */
		pstSize->u32Width  = 1280;
		pstSize->u32Height = 720;
		break;

	case PIC_1080P:  /* 1920 * 1080 */
		pstSize->u32Width  = 1920;
		pstSize->u32Height = 1080;
		break;

	case PIC_1088:  /* 1920 * 1088*/
		pstSize->u32Width  = 1920;
		pstSize->u32Height = 1088;
		break;

	case PIC_1440P:  /* 2560 * 1440 */
		pstSize->u32Width  = 2560;
		pstSize->u32Height = 1440;
		break;

	case PIC_2304x1296:
		pstSize->u32Width  = 2304;
		pstSize->u32Height = 1296;
		break;

	case PIC_2592x1520:
		pstSize->u32Width  = 2592;
		pstSize->u32Height = 1520;
		break;

	case PIC_2560x1600:
		pstSize->u32Width  = 2560;
		pstSize->u32Height = 1600;
		break;

	case PIC_2592x1944:
		pstSize->u32Width  = 2592;
		pstSize->u32Height = 1944;
		break;

	case PIC_2592x1536:
		pstSize->u32Width  = 2592;
		pstSize->u32Height = 1536;
		break;

	case PIC_2688x1520:
		pstSize->u32Width  = 2688;
		pstSize->u32Height = 1520;
		break;

	case PIC_2716x1524:
		pstSize->u32Width  = 2716;
		pstSize->u32Height = 1524;
		break;

	case PIC_2880x1620:
		pstSize->u32Width  = 2880;
		pstSize->u32Height = 1620;
		break;

	case PIC_3844x1124:
		pstSize->u32Width  = 3844;
		pstSize->u32Height = 1124;
		break;

	case PIC_3840x2160:
		pstSize->u32Width  = 3840;
		pstSize->u32Height = 2160;
		break;

	case PIC_3000x3000:
		pstSize->u32Width  = 3000;
		pstSize->u32Height = 3000;
		break;

	case PIC_4000x3000:
		pstSize->u32Width  = 4000;
		pstSize->u32Height = 3000;
		break;

	case PIC_4096x2160:
		pstSize->u32Width  = 4096;
		pstSize->u32Height = 2160;
		break;

	case PIC_3840x8640:
		pstSize->u32Width = 3840;
		pstSize->u32Height = 8640;
		break;

	case PIC_7688x1124:
		pstSize->u32Width = 7688;
		pstSize->u32Height = 1124;
		break;

	case PIC_640x480:
		pstSize->u32Width = 640;
		pstSize->u32Height = 480;
		break;
	case PIC_479P:  /* 632 * 479 */
		pstSize->u32Width  = 632;
		pstSize->u32Height = 479;
		break;
	case PIC_288P:  /* 384 * 288 */
		pstSize->u32Width  = 384;
		pstSize->u32Height = 288;
		break;
	default:
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 i;

	memset(pstViConfig, 0, sizeof(*pstViConfig));

	for (i = 0; i < VI_MAX_DEV_NUM; i++) {
		pstViConfig->astViInfo[i].stSnsInfo.s32SnsId = i;
		pstViConfig->astViInfo[i].stSnsInfo.s32BusId = i;
		pstViConfig->astViInfo[i].stSnsInfo.MipiDev  = i;
		memset(&pstViConfig->astViInfo[i].stSnapInfo, 0, sizeof(SAMPLE_SNAP_INFO_S));
		pstViConfig->astViInfo[i].stPipeInfo.bMultiPipe = CVI_FALSE;
		pstViConfig->astViInfo[i].stPipeInfo.bVcNumCfged = CVI_FALSE;
	}

	pstViConfig->astViInfo[0].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_60FPS_12BIT;
	pstViConfig->astViInfo[1].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_60FPS_12BIT;
}

CVI_S32 SAMPLE_COMM_VI_GetDevAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S *pstViDevAttr)
{
	switch (enSnsType) {
	default:
	case SONY_IMX290_MIPI_1M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX297_1M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
#ifdef FPGA_PORTING
	case SONY_IMX327_MIPI_1M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX327_1M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX327_1M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
#endif
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_60FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX327_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX327_2M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_60FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX334_8M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX334_8M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_60FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_5M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_5M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_60FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_4M_1600P_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_4M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_4M_1600P_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_2M_60FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX335_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX385_MIPI_2M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX385_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX385_2M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case OV_OV4689_MIPI_4M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_OV4689_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_OS08A20_8M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_OS08A20_8M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_OS08A20_5M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_OS08A20_5M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case OV_OS08A20_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_OS08A20_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_OS08A20_4M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SOI_F23_MIPI_2M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_F23_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_F35_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_F35_2M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SOI_H65_MIPI_1M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_H65_1M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SOI_K04_MIPI_4M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_K04_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case PICO640_THERMAL_479P:
		memcpy(pstViDevAttr, &DEV_ATTR_PICO_640_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case PICO384_THERMAL_384X288:
		memcpy(pstViDevAttr, &DEV_ATTR_PICO_384_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX327_SUBLVDS_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX307_SUBLVDS_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_VIVO_MCS369Q_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case VIVO_MCS369_2M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_VIVO_MCS369_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case VIVO_MM308M2_2M_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_VIVO_MM308M2_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case NEXTCHIP_N5_2M_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_NEXTCHIP_N5_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case NEXTCHIP_N6_2M_4CH_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_NEXTCHIP_N6_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case NEXTCHIP_N5_1M_2CH_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_NEXTCHIP_N5_1M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
	case SMS_SC3335_SLAVE_MIPI_3M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC3335_3M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC3336_MIPI_3M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC3336_3M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC2335_MIPI_2M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC2335_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC2336_MIPI_2M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC2336_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC4336_MIPI_4M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC4336_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT:
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_SC200AI_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_SC500AI_5M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_SC500AI_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC501AI_2L_MIPI_5M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC501AI_2L_5M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC401AI_MIPI_4M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC401AI_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case PIXELPLUS_PR2020_1M_25FPS_8BIT:
	case PIXELPLUS_PR2020_1M_30FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_PIXELPLUS_PR2020_1M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_30FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_PIXELPLUS_PR2020_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case PIXELPLUS_PR2100_2M_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_PIXELPLUS_PR2100_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
#ifdef ARCH_CV183X
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_PIXELPLUS_PR2100_2M_2CH_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_PIXELPLUS_PR2100_2M_4CH_BASE, sizeof(VI_DEV_ATTR_S));
		break;
#else
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_PIXELPLUS_PR2100_2M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->enWorkMode = VI_WORK_MODE_2Multiplex;
		break;
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_PIXELPLUS_PR2100_2M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->enWorkMode = VI_WORK_MODE_4Multiplex;
		break;
#endif
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC8238_8M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_SC8238_8M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC4210_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_SC4210_4M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_GC2053_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_GC2093_2M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_GC2093_2M_BASE, sizeof(VI_DEV_ATTR_S));
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_GC4653_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_GC1054_1M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SONY_IMX347_MIPI_4M_60FPS_12BIT:
	case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
		memcpy(pstViDevAttr, &DEV_ATTR_IMX347_4M_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC035HGS_MIPI_480P_120FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC035HGS_480P_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	case SMS_SC035GS_MIPI_480P_120FPS_12BIT:
		memcpy(pstViDevAttr, &DEV_ATTR_SC035GS_480P_BASE, sizeof(VI_DEV_ATTR_S));
		break;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_GetChnAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S *pstChnAttr)
{
	switch (enSnsType) {
	default:
	case SONY_IMX290_MIPI_1M_30FPS_12BIT:
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
#ifdef FPGA_PORTING
	case SONY_IMX327_MIPI_1M_30FPS_10BIT:
	case SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1:
#endif
		memcpy(pstChnAttr, &CHN_ATTR_1280x720_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_MIPI_2M_60FPS_12BIT:
		memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
	case SONY_IMX307_MIPI_2M_60FPS_12BIT:
	case SONY_IMX335_MIPI_2M_60FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_5M_60FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2592x1944_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_60FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2560x1600_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SONY_IMX385_MIPI_2M_30FPS_12BIT:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_2592x1944_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case OV_OS08A20_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SOI_F23_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SMS_SC2335_MIPI_2M_30FPS_10BIT:
	case SMS_SC2336_MIPI_2M_30FPS_10BIT:
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT:
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SOI_H65_MIPI_1M_30FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_1280x720_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case OV_OV4689_MIPI_4M_30FPS_10BIT:
	case SOI_K04_MIPI_4M_30FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2688x1520_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case PIXELPLUS_PR2020_1M_25FPS_8BIT:
	case PIXELPLUS_PR2020_1M_30FPS_8BIT:
	case NEXTCHIP_N5_1M_2CH_25FPS_8BIT:
		memcpy(pstChnAttr, &CHN_ATTR_1280x720_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case PICO640_THERMAL_479P:
		memcpy(pstChnAttr, &CHN_ATTR_632x479_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case PICO384_THERMAL_384X288:
		memcpy(pstChnAttr, &CHN_ATTR_384x288_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case VIVO_MM308M2_2M_25FPS_8BIT:
	case NEXTCHIP_N5_2M_25FPS_8BIT:
	case NEXTCHIP_N6_2M_4CH_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_30FPS_8BIT:
	case PIXELPLUS_PR2100_2M_25FPS_8BIT:
	case VIVO_MCS369_2M_30FPS_12BIT:
		memcpy(pstChnAttr, &CHN_ATTR_1920x1080_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2560x1440_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
	case SMS_SC3335_SLAVE_MIPI_3M_30FPS_10BIT:
	case SMS_SC3336_MIPI_3M_30FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2304x1296_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SMS_SC401AI_MIPI_4M_30FPS_10BIT:
	case SMS_SC4336_MIPI_4M_30FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
#ifdef ARCH_CV183X
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
		memcpy(pstChnAttr, &CHN_ATTR_3844x1124_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		memcpy(pstChnAttr, &CHN_ATTR_7688x1124_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
#else
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		memcpy(pstChnAttr, &CHN_ATTR_1920x1080_422_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
#endif
	case SONY_IMX347_MIPI_4M_60FPS_12BIT:
	case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
		memcpy(pstChnAttr, &CHN_ATTR_2688x1520_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SMS_SC035HGS_MIPI_480P_120FPS_12BIT:
	case SMS_SC035GS_MIPI_480P_120FPS_12BIT:
		memcpy(pstChnAttr, &CHN_ATTR_640x480_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SMS_SC501AI_2L_MIPI_5M_30FPS_10BIT:
		memcpy(pstChnAttr, &CHN_ATTR_2880x1620_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
		break;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_ResetSensor(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		s32Ret = CVI_MIPI_SetSensorReset(devno, 1);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d reset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_ResetMipi(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		s32Ret = CVI_MIPI_SetMipiReset(devno, 1);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "mipi %d reset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_UnresetSensor(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		s32Ret = CVI_MIPI_SetSensorReset(devno, 0);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d unreset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_UnresetMipi(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		s32Ret = CVI_MIPI_SetMipiReset(devno, 0);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("mipi %d unreset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_SetMipiAttr(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;
	SAMPLE_SNS_TYPE_E enSnsType;

	const ISP_SNS_OBJ_S *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		enSnsType = g_enSnsType[u32SnsId];
		/* need to invert the clk for timnig issue. */
		if (enSnsType == VIVO_MCS369Q_4M_30FPS_12BIT ||
				enSnsType == VIVO_MCS369_2M_30FPS_12BIT)
			CVI_MIPI_SetClkEdge(ViPipe, 0);
		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		pstSnsObj->pfnGetRxAttr(ViPipe, &stDevAttr);
		CVI_MIPI_SetMipiAttr(ViPipe, (CVI_VOID *)&stDevAttr);
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_EnableSensorClock(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		s32Ret = CVI_MIPI_SetSensorClock(devno, 1);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d clock enable failed!\n", i);
			return s32Ret;
		}
	}

	return CVI_SUCCESS;
}

struct VI_PM_DATA_S {
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	CVI_S32 s32DevNo;
};

static struct VI_PM_DATA_S ViPmData[VI_MAX_DEV_NUM] = { 0 };

static CVI_S32 SAMPLE_COMM_VI_SuspendSensor(CVI_VOID *pvData)
{
	struct VI_PM_DATA_S *pstPmData = (struct VI_PM_DATA_S *)pvData;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	ISP_SNS_OBJ_S *pstSnsObj;

	if (!pvData) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pvData is NULL!\n");
		return CVI_FAILURE;
	}
	ViPipe = pstPmData->ViPipe;
	u32SnsId = pstPmData->u32SnsId;

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	if (!pstSnsObj) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d object get failed!\n", u32SnsId);
		return CVI_FAILURE;
	}
	if (!pstSnsObj->pfnStandby) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "no sensor %d object standby callback\n", u32SnsId);
		return CVI_FAILURE;
	}
	pstSnsObj->pfnStandby(ViPipe);

	return CVI_SUCCESS;
}

static CVI_S32 SAMPLE_COMM_VI_ResumeSensor(CVI_VOID *pvData)
{
	struct VI_PM_DATA_S *pstPmData = (struct VI_PM_DATA_S *)pvData;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	ISP_SNS_OBJ_S *pstSnsObj;
	ISP_SENSOR_EXP_FUNC_S pfnSnsExp;

	if (!pvData) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pvData is NULL!\n");
		return CVI_FAILURE;
	}
	ViPipe = pstPmData->ViPipe;
	u32SnsId = pstPmData->u32SnsId;

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	if (!pstSnsObj) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d object get failed!\n", u32SnsId);
		return CVI_FAILURE;
	}

	if (!pstSnsObj->pfnExpSensorCb) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "no sensor %d object ExpSensor callback\n", u32SnsId);
		return CVI_FAILURE;
	}

	memset(&pfnSnsExp, 0, sizeof(pfnSnsExp));
	pstSnsObj->pfnExpSensorCb(&pfnSnsExp);
	if (!pfnSnsExp.pfn_cmos_sensor_init) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "no sensor %d object pfn_cmos_sensor_init callback\n", u32SnsId);
		return CVI_FAILURE;
	}
	pfnSnsExp.pfn_cmos_sensor_init(ViPipe);

	return CVI_SUCCESS;
}

static CVI_S32 SAMPLE_COMM_VI_SuspendMipi(CVI_VOID *pvData)
{
	struct VI_PM_DATA_S *pstPmData = (struct VI_PM_DATA_S *)pvData;
	CVI_S32 devno;
	CVI_S32 s32Ret = 0;

	if (!pvData) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pvData is NULL!\n");
		return CVI_FAILURE;
	}
	devno = pstPmData->s32DevNo;

	s32Ret = CVI_MIPI_SetSensorClock(devno, 0);
	if (s32Ret != CVI_SUCCESS)
		CVI_TRACE_LOG(CVI_DBG_ERR, "dev %d clock enable failed!\n", devno);

	return s32Ret;
}

static CVI_S32 SAMPLE_COMM_VI_ResumeMipi(CVI_VOID *pvData)
{
	struct VI_PM_DATA_S *pstPmData = (struct VI_PM_DATA_S *)pvData;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	CVI_S32 devno;
	ISP_SNS_OBJ_S *pstSnsObj;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	SAMPLE_SNS_TYPE_E enSnsType;
	CVI_S32 s32Ret = 0;

	if (!pvData) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pvData is NULL!\n");
		return CVI_FAILURE;
	}

	ViPipe = pstPmData->ViPipe;
	u32SnsId = pstPmData->u32SnsId;
	devno = pstPmData->s32DevNo;

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	if (!pstSnsObj) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d object get failed!\n", u32SnsId);
		return CVI_FAILURE;
	}

	/* get mipi-rx attribute from sensor driver. */
	pstSnsObj->pfnGetRxAttr(ViPipe, &stDevAttr);
	/* sensor reset */
	s32Ret = CVI_MIPI_SetSensorReset(devno, 1);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d reset failed!\n", u32SnsId);
		return s32Ret;
	}
	/* mipi-rx reset */
	s32Ret = CVI_MIPI_SetMipiReset(devno, 1);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "mipi %d reset failed!\n", u32SnsId);
		return s32Ret;
	}
	/* set mipi-rx attribute */
	enSnsType = g_enSnsType[u32SnsId];
	/* need to invert the clk for timnig issue. */
	if (enSnsType == VIVO_MCS369Q_4M_30FPS_12BIT ||
			enSnsType == VIVO_MCS369_2M_30FPS_12BIT)
		CVI_MIPI_SetClkEdge(ViPipe, 0);
	CVI_MIPI_SetMipiAttr(ViPipe, (CVI_VOID *)&stDevAttr);
	/* enable sensor clock. */
	s32Ret = CVI_MIPI_SetSensorClock(devno, 1);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d clock enable failed!\n", u32SnsId);
		return s32Ret;
	}
	usleep(20);
	/* sensor unreset */
	s32Ret = CVI_MIPI_SetSensorReset(devno, 0);
	if (s32Ret != CVI_SUCCESS)
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d reset failed!\n", u32SnsId);
	usleep(10000);

	return s32Ret;
}

static VI_PM_OPS_S vi_ops = {
	.pfnSnsSuspend = SAMPLE_COMM_VI_SuspendSensor,
	.pfnSnsResume = SAMPLE_COMM_VI_ResumeSensor,
	.pfnMipiSuspend = SAMPLE_COMM_VI_SuspendMipi,
	.pfnMipiResume = SAMPLE_COMM_VI_ResumeMipi,
};

CVI_S32 SAMPLE_COMM_VI_StartSensor(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret, i;
	CVI_U32 u32SnsId;
	VI_PIPE ViPipe;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		s32Ret = SAMPLE_COMM_ISP_SetSnsObj(u32SnsId, pstViInfo->stSnsInfo.enSnsType);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "update sensor obj(%d) failed!\n", u32SnsId);
			return s32Ret;
		}
		s32Ret = SAMPLE_COMM_ISP_SetSnsInit(u32SnsId, pstViInfo->stSnsInfo.u8HwSync);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "update sensor(%d) hwsync failed !\n", u32SnsId);
			return s32Ret;
		}
		s32Ret = SAMPLE_COMM_ISP_PatchSnsObj(u32SnsId, &pstViInfo->stSnsInfo);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "patch rx attr(%d) failed!\n", u32SnsId);
			return s32Ret;
		}
		s32Ret = SAMPLE_COMM_ISP_Sensor_Regiter_callback(ViPipe, u32SnsId, pstViInfo->stSnsInfo.s32BusId,
								pstViInfo->stSnsInfo.s32SnsI2cAddr);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d register callback failed!\n", i);
			return s32Ret;
		}
	}
	s32Ret = SAMPLE_COMM_ISP_SetSensorMode(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d register callback failed!\n", i);
		return s32Ret;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = CVI_SUCCESS, i;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;
	const ISP_SNS_OBJ_S *pstSnsObj;

	/*TODO@CF. Need add sample function.*/
	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		pstSnsObj->pfnGetRxAttr(ViPipe, &stDevAttr);
		SAMPLE_PRT("sensor %d stDevAttr.devno %d\n", i, stDevAttr.devno);
		pstViInfo->stSnsInfo.MipiDev = stDevAttr.devno;
	}
	s32Ret = SAMPLE_COMM_VI_ResetSensor(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "ResetSensor failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_ResetMipi(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "ResetMipi failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_SetMipiAttr(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_SetMipiAttr failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_EnableSensorClock(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "EnableSensorClock failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	usleep(20);
	s32Ret = SAMPLE_COMM_VI_UnresetSensor(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "UnresetSensor failed! with %#x!\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_SensorProbe(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = CVI_SUCCESS, i;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;
	const ISP_SNS_OBJ_S *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		if (pstSnsObj->pfnSnsProbe) {
			s32Ret = pstSnsObj->pfnSnsProbe(ViPipe);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "cannot probe the sensorObj(%p) SnsId(%d)\n",
					pstSnsObj, u32SnsId);
				return s32Ret;
			}
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StartDev(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32             s32Ret;
	VI_DEV              ViDev;
	SAMPLE_SNS_TYPE_E   enSnsType;
	VI_DEV_ATTR_S       stViDevAttr;
	ISP_PUB_ATTR_S      pstPubAttr;

	ViDev       = pstViInfo->stDevInfo.ViDev;
	enSnsType   = pstViInfo->stSnsInfo.enSnsType;

	SAMPLE_COMM_VI_GetDevAttrBySns(enSnsType, &stViDevAttr);
	SAMPLE_COMM_ISP_GetIspAttrBySns(enSnsType, &pstPubAttr);
	stViDevAttr.stWDRAttr.enWDRMode = pstViInfo->stDevInfo.enWDRMode;
	stViDevAttr.snrFps = (CVI_U32)pstPubAttr.f32FrameRate;

	s32Ret = CVI_VI_SetDevAttr(ViDev, &stViDevAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_SetDevAttr failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_EnableDev(ViDev);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_EnableDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_StopDev(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32 s32Ret;
	VI_DEV ViDev;

	ViDev   = pstViInfo->stDevInfo.ViDev;
	s32Ret  = CVI_VI_DisableDev(ViDev);

	CVI_VI_UnRegChnFlipMirrorCallBack(0, ViDev);
	CVI_VI_UnRegPmCallBack(ViDev);
	memset(&ViPmData[ViDev], 0, sizeof(struct VI_PM_DATA_S));

	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_DisableDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_BindPipeDev(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32             i;
	CVI_S32             s32PipeCnt = 0;
	CVI_S32             s32Ret;
	VI_DEV_BIND_PIPE_S  stDevBindPipe = {0};

	for (i = 0; i < 4; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			stDevBindPipe.PipeId[s32PipeCnt] = pstViInfo->stPipeInfo.aPipe[i];
			s32PipeCnt++;
			stDevBindPipe.u32Num = s32PipeCnt;
		}
	}

	s32Ret = CVI_VI_SetDevBindPipe(pstViInfo->stDevInfo.ViDev, &stDevBindPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_SetDevBindPipe failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	return s32Ret;
}

/******************************************************************************
 * funciton : Get enSize by diffrent sensor
 ******************************************************************************/
CVI_S32 SAMPLE_COMM_VI_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E *penSize)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	if (!penSize)
		return CVI_FAILURE;

	switch (enMode) {
	case SONY_IMX290_MIPI_1M_30FPS_12BIT:
	case SOI_H65_MIPI_1M_30FPS_10BIT:
	case PIXELPLUS_PR2020_1M_25FPS_8BIT:
	case PIXELPLUS_PR2020_1M_30FPS_8BIT:
	case NEXTCHIP_N5_1M_2CH_25FPS_8BIT:
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
#ifdef FPGA_PORTING
	case SONY_IMX327_MIPI_1M_30FPS_10BIT:
	case SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1:
#endif
		*penSize = PIC_720P;
		break;
	case SONY_IMX290_MIPI_2M_60FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SOI_F23_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case VIVO_MM308M2_2M_25FPS_8BIT:
	case NEXTCHIP_N5_2M_25FPS_8BIT:
	case NEXTCHIP_N6_2M_4CH_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_30FPS_8BIT:
	case PIXELPLUS_PR2100_2M_25FPS_8BIT:
	case VIVO_MCS369_2M_30FPS_12BIT:
	case SMS_SC2335_MIPI_2M_30FPS_10BIT:
	case SMS_SC2336_MIPI_2M_30FPS_10BIT:
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT:
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX327_MIPI_2M_60FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
	case SONY_IMX307_MIPI_2M_60FPS_12BIT:
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
	case SONY_IMX335_MIPI_2M_60FPS_10BIT:
		*penSize = PIC_1080P;
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		*penSize = PIC_3840x2160;
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_5M_60FPS_10BIT:
		*penSize = PIC_2592x1944;
		break;
	case SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1:
		*penSize = PIC_2560x1600;
		break;
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_60FPS_10BIT:
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SMS_SC401AI_MIPI_4M_30FPS_10BIT:
	case SMS_SC4336_MIPI_4M_30FPS_10BIT:
		*penSize = PIC_1440P;
		break;
	case PICO640_THERMAL_479P:
		*penSize = PIC_479P;
		break;
	case PICO384_THERMAL_384X288:
		*penSize = PIC_288P;
		break;
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		*penSize = PIC_1440P;
		break;
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
	case SMS_SC3335_SLAVE_MIPI_3M_30FPS_10BIT:
	case SMS_SC3336_MIPI_3M_30FPS_10BIT:
		*penSize = PIC_2304x1296;
		break;
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		*penSize = PIC_3840x2160;
		break;
#ifdef ARCH_CV183X
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
		*penSize = PIC_3844x1124;
		break;
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		*penSize = PIC_7688x1124;
		break;
#else
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		*penSize = PIC_1080P;
		break;
#endif
	case OV_OV4689_MIPI_4M_30FPS_10BIT:
	case SOI_K04_MIPI_4M_30FPS_10BIT:
	case SONY_IMX347_MIPI_4M_60FPS_12BIT:
	case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
		*penSize = PIC_2688x1520;
		break;
	case SMS_SC035HGS_MIPI_480P_120FPS_12BIT:
	case SMS_SC035GS_MIPI_480P_120FPS_12BIT:
		*penSize = PIC_640x480;
		break;
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SMS_SC501AI_2L_MIPI_5M_30FPS_10BIT:
		*penSize = PIC_2880x1620;
		break;
	default:
		s32Ret = CVI_FAILURE;
		break;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StartViChn(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32             i;
	CVI_S32             s32Ret = CVI_SUCCESS;
	VI_PIPE             ViPipe = 0;
	VI_CHN              ViChn = 0;
	VI_DEV              ViDev = 0;
	CVI_U32             u32SnsId = 0;
	VI_CHN_ATTR_S       stChnAttr;
	VI_DEV_ATTR_S       stViDevAttr;
	ISP_SNS_OBJ_S       *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		if (i < VI_MAX_DEV_NUM) {
			ViPipe	    = pstViConfig->astViInfo[i].stPipeInfo.aPipe[0];
			ViChn	    = pstViConfig->astViInfo[i].stChnInfo.ViChn;
			ViDev	    = pstViConfig->astViInfo[i].stDevInfo.ViDev;
			u32SnsId    = pstViConfig->astViInfo[i].stSnsInfo.s32SnsId;
			pstSnsObj   = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

			SAMPLE_COMM_VI_GetDevAttrBySns(pstViConfig->astViInfo[i].stSnsInfo.enSnsType, &stViDevAttr);
			SAMPLE_COMM_VI_GetChnAttrBySns(pstViConfig->astViInfo[i].stSnsInfo.enSnsType, &stChnAttr);
			stChnAttr.enDynamicRange = pstViConfig->astViInfo[i].stChnInfo.enDynamicRange;
			stChnAttr.enVideoFormat  = pstViConfig->astViInfo[i].stChnInfo.enVideoFormat;
			stChnAttr.enCompressMode = pstViConfig->astViInfo[i].stChnInfo.enCompressMode;
			stChnAttr.enPixelFormat = pstViConfig->astViInfo[i].stChnInfo.enPixFormat;
			/* fill the sensor orientation */
			if (pstViConfig->astViInfo[i].stSnsInfo.u8Orien <= 3) {
				stChnAttr.bMirror = pstViConfig->astViInfo[i].stSnsInfo.u8Orien & 0x1;
				stChnAttr.bFlip = pstViConfig->astViInfo[i].stSnsInfo.u8Orien & 0x2;
			}

			s32Ret = CVI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_SetChnAttr failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}

			if (pstSnsObj && pstSnsObj->pfnMirrorFlip)
				CVI_VI_RegChnFlipMirrorCallBack(ViPipe, ViDev, (void *)pstSnsObj->pfnMirrorFlip);

			/* register the power management ops. */
			ViPmData[ViDev].ViPipe = ViPipe;
			ViPmData[ViDev].u32SnsId = u32SnsId;
			ViPmData[ViDev].s32DevNo = pstViConfig->astViInfo[i].stSnsInfo.MipiDev;
			s32Ret = CVI_VI_RegPmCallBack(ViDev, &vi_ops, (CVI_VOID *)&ViPmData[ViDev]);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_RegPmCallBack failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}

			s32Ret = CVI_VI_EnableChn(ViPipe, ViChn);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_EnableChn failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StopViChn(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32             s32Ret = CVI_SUCCESS;
	VI_PIPE             ViPipe = 0;
	VI_CHN              ViChn;
	VI_VPSS_MODE_E      enMastPipeMode;

	ViChn  = pstViInfo->stChnInfo.ViChn;

	if (ViChn < VI_MAX_CHN_NUM) {
		enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;

		if (enMastPipeMode == VI_OFFLINE_VPSS_OFFLINE
		    || enMastPipeMode == VI_ONLINE_VPSS_OFFLINE) {
			s32Ret = CVI_VI_DisableChn(ViPipe, ViChn);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_DisableChn failed with %#x!\n",
								s32Ret);
				return s32Ret;
			}
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_CreateIsp(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	#define USE_OLDAPI_LOADPARA		0

	CVI_S32 i;
	CVI_S32 s32ViNum;
	CVI_S32 s32Ret = CVI_SUCCESS;

	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	if (!pstViConfig) {
		SAMPLE_PRT("%s: null ptr\n", __func__);
		return CVI_FAILURE;
	}

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		s32ViNum  = pstViConfig->as32WorkingViId[i];
		pstViInfo = &pstViConfig->astViInfo[s32ViNum];

		s32Ret = SAMPLE_COMM_VI_StartIsp(pstViInfo);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("SAMPLE_COMM_VI_StartIsp failed !\n");
			return CVI_FAILURE;
		}

		#if USE_OLDAPI_LOADPARA
		s32Ret = SAMPLE_COMM_BIN_ReadBlockParaFrombin(CVI_BIN_ID_ISP0 + i);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_WARN, "read block(%d) para fail: %#x!\n", CVI_BIN_ID_ISP0 + i, s32Ret);
		}
		#else
		if (i == (pstViConfig->s32WorkingViNum - 1)) {
			s32Ret = SAMPLE_COMM_BIN_ReadParaFrombin();
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_WARN, "read para from file fail: %#x,use default para!\n",
							  s32Ret);
			}
		}
		#endif

		s32Ret = SAMPLE_COMM_ISP_Run(s32ViNum);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "ISP_Run failed with %#x!\n", s32Ret);
			return s32Ret;
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_StartIsp(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32 s32Ret = 0, i;
	VI_PIPE ViPipe = 0;
	ISP_PUB_ATTR_S stPubAttr;
	ISP_STATISTICS_CFG_S stsCfg = {0};
	ISP_BIND_ATTR_S stBindAttr;

	for (i = 0; i < WDR_MAX_PIPE_NUM; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  &&
			pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			ViPipe = pstViInfo->stPipeInfo.aPipe[0];

			SAMPLE_COMM_ISP_Aelib_Callback(ViPipe);
			SAMPLE_COMM_ISP_Awblib_Callback(ViPipe);
			#if ENABLE_AF_LIB
			SAMPLE_COMM_ISP_Aflib_Callback(ViPipe);
			#endif

			snprintf(stBindAttr.stAeLib.acLibName, sizeof(CVI_AE_LIB_NAME), "%s", CVI_AE_LIB_NAME);
			stBindAttr.stAeLib.s32Id = ViPipe;
			stBindAttr.sensorId = 0;
			snprintf(stBindAttr.stAwbLib.acLibName, sizeof(CVI_AWB_LIB_NAME), "%s", CVI_AWB_LIB_NAME);
			stBindAttr.stAwbLib.s32Id = ViPipe;
			#if ENABLE_AF_LIB
			snprintf(stBindAttr.stAfLib.acLibName, sizeof(CVI_AF_LIB_NAME), "%s", CVI_AF_LIB_NAME);
			stBindAttr.stAfLib.s32Id = ViPipe;
			#endif
			s32Ret = CVI_ISP_SetBindAttr(ViPipe, &stBindAttr);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "Bind Algo failed with %#x!\n", s32Ret);
			}
			s32Ret = CVI_ISP_MemInit(ViPipe);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "Init Ext memory failed with %#x!\n", s32Ret);
				return s32Ret;
			}
			SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPubAttr);
			s32Ret = CVI_ISP_SetPubAttr(ViPipe, &stPubAttr);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "SetPubAttr failed with %#x!\n", s32Ret);
				return s32Ret;
			}

			stsCfg.stAECfg.stCrop[0].bEnable = 0;
			stsCfg.stAECfg.stCrop[0].u16X = stsCfg.stAECfg.stCrop[0].u16Y = 0;
			stsCfg.stAECfg.stCrop[0].u16W = stPubAttr.stWndRect.u32Width;
			stsCfg.stAECfg.stCrop[0].u16H = stPubAttr.stWndRect.u32Height;
			#ifdef ARCH_CV183X
			stsCfg.stAECfg.stCrop[1].bEnable = 0;
			stsCfg.stAECfg.stCrop[1].u16X = stsCfg.stAECfg.stCrop[1].u16Y = 0;
			stsCfg.stAECfg.stCrop[1].u16W = stPubAttr.stWndRect.u32Width;
			stsCfg.stAECfg.stCrop[1].u16H = stPubAttr.stWndRect.u32Height;
			#endif

			stsCfg.stWBCfg.u16ZoneRow = AWB_ZONE_ORIG_ROW;
			stsCfg.stWBCfg.u16ZoneCol = AWB_ZONE_ORIG_COLUMN;
			stsCfg.stWBCfg.stCrop.bEnable = 0;
			stsCfg.stWBCfg.stCrop.u16X = stsCfg.stWBCfg.stCrop.u16Y = 0;
			stsCfg.stWBCfg.stCrop.u16W = stPubAttr.stWndRect.u32Width;
			stsCfg.stWBCfg.stCrop.u16H = stPubAttr.stWndRect.u32Height;
			stsCfg.stWBCfg.u16BlackLevel = 0;
			stsCfg.stWBCfg.u16WhiteLevel = 4095;
			stsCfg.stFocusCfg.stConfig.bEnable = 1;
			stsCfg.stFocusCfg.stConfig.u8HFltShift = 1;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[0] = 1;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[1] = 2;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[2] = 3;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[3] = 5;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[4] = 10;
			stsCfg.stFocusCfg.stConfig.stRawCfg.PreGammaEn = 0;
			stsCfg.stFocusCfg.stConfig.stPreFltCfg.PreFltEn = 1;
			stsCfg.stFocusCfg.stConfig.u16Hwnd = 17;
			stsCfg.stFocusCfg.stConfig.u16Vwnd = 15;
			stsCfg.stFocusCfg.stConfig.stCrop.bEnable = 0;
			// AF offset and size has some limitation.
			stsCfg.stFocusCfg.stConfig.stCrop.u16X = AF_XOFFSET_MIN;
			stsCfg.stFocusCfg.stConfig.stCrop.u16Y = AF_YOFFSET_MIN;
			stsCfg.stFocusCfg.stConfig.stCrop.u16W = stPubAttr.stWndRect.u32Width - AF_XOFFSET_MIN * 2;
			stsCfg.stFocusCfg.stConfig.stCrop.u16H = stPubAttr.stWndRect.u32Height - AF_YOFFSET_MIN * 2;
			//Horizontal HP0
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[0] = 0;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[1] = 0;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[2] = 13;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[3] = 24;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[4] = 0;
			//Horizontal HP1
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[0] = 1;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[1] = 2;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[2] = 4;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[3] = 8;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[4] = 0;
			//Vertical HP
			stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[0] = 13;
			stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[1] = 24;
			stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[2] = 0;
			stsCfg.unKey.bit1FEAeGloStat = stsCfg.unKey.bit1FEAeLocStat =
				stsCfg.unKey.bit1AwbStat1 = stsCfg.unKey.bit1AwbStat2 = stsCfg.unKey.bit1FEAfStat = 1;
#ifndef FPGA_PORTING
			s32Ret = CVI_ISP_SetStatisticsConfig(ViPipe, &stsCfg);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "ISP Set Statistic failed with %#x!\n", s32Ret);
				return s32Ret;
			}
#endif
			s32Ret = CVI_ISP_Init(ViPipe);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "ISP Init failed with %#x!\n", s32Ret);
				return s32Ret;
			}
		}
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_DestroyIsp(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 i;
	CVI_S32 s32ViNum;
	CVI_S32 s32Ret = CVI_SUCCESS;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	if (!pstViConfig) {
		SAMPLE_PRT("%s: null ptr\n", __func__);
		return CVI_FAILURE;
	}

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		s32ViNum  = pstViConfig->as32WorkingViId[i];
		pstViInfo = &pstViConfig->astViInfo[s32ViNum];

		s32Ret = SAMPLE_COMM_VI_StopIsp(pstViInfo);

		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("SAMPLE_COMM_VI_StopIsp failed !\n");
			return CVI_FAILURE;
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_StopIsp(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32 ret = CVI_SUCCESS;
	VI_PIPE ViPipe;

	for (CVI_U32 i = 0; i < WDR_MAX_PIPE_NUM; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			ViPipe = pstViInfo->stPipeInfo.aPipe[i];
			SAMPLE_COMM_ISP_Stop(ViPipe);
		}
	}

	return ret;
}

static CVI_S32 SAMPLE_COMM_VI_StopSingleViPipe(VI_PIPE ViPipe)
{
	CVI_S32  s32Ret;

	s32Ret = CVI_VI_StopPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_StopPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_DestroyPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_DestroyPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StopViPipe(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32  i, ret = CVI_SUCCESS;
	VI_PIPE ViPipe;

	for (i = 0; i < WDR_MAX_PIPE_NUM; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			ViPipe = pstViInfo->stPipeInfo.aPipe[i];
			ret = SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
			if (ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_StopViPipe is fail\n");
				return ret;
			}
		}
	}

	return CVI_SUCCESS;
}

static CVI_S32 SAMPLE_COMM_VI_DestroySingleVi(SAMPLE_VI_INFO_S *pstViInfo)
{
	SAMPLE_COMM_VI_StopViChn(pstViInfo);

	SAMPLE_COMM_VI_StopViPipe(pstViInfo);

	SAMPLE_COMM_VI_StopDev(pstViInfo);

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_DestroyVi(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32           i;
	CVI_S32           s32ViNum;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	if (!pstViConfig) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "null ptr\n");
		return CVI_FAILURE;
	}

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		s32ViNum  = pstViConfig->as32WorkingViId[i];
		pstViInfo = &pstViConfig->astViInfo[s32ViNum];

		SAMPLE_COMM_VI_DestroySingleVi(pstViInfo);
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_OPEN(CVI_VOID)
{
	CVI_S32 s32ret = CVI_SUCCESS;

	s32ret = CVI_SYS_VI_Open();
	if (s32ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI reopen err\n");
	}

	return s32ret;
}

CVI_S32 SAMPLE_COMM_VI_CLOSE(CVI_VOID)
{
	CVI_S32 s32ret = CVI_SUCCESS;

	s32ret = CVI_SYS_VI_Close();
	if (s32ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI close err\n");
	}

	return s32ret;
}

static const char *snsr_type_name[SAMPLE_SNS_TYPE_BUTT] = {
	/* ------ LINEAR BEGIN ------*/
	"SONY_IMX290_MIPI_1M_30FPS_12BIT",
	"SONY_IMX290_MIPI_2M_60FPS_12BIT",
#ifdef FPGA_PORTING
	"SONY_IMX327_MIPI_1M_30FPS_10BIT",
#endif
	"SONY_IMX327_MIPI_2M_30FPS_12BIT",
	"SONY_IMX307_MIPI_2M_30FPS_12BIT",
	"SONY_IMX327_2L_MIPI_2M_30FPS_12BIT",
	"SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT",
	"SONY_IMX307_2L_MIPI_2M_30FPS_12BIT",
	"SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT",
	"OV_OV4689_MIPI_4M_30FPS_10BIT",
	"OV_OS08A20_MIPI_8M_30FPS_10BIT",
	"OV_OS08A20_MIPI_5M_30FPS_10BIT",
	"OV_OS08A20_MIPI_4M_30FPS_10BIT",
	"OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT",
	"OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT",
	"OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT",
	"SOI_F23_MIPI_2M_30FPS_10BIT",
	"SOI_F35_MIPI_2M_30FPS_10BIT",
	"SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT",
	"SOI_H65_MIPI_2M_30FPS_10BIT",
	"SOI_K04_MIPI_4M_30FPS_10BIT",
	"PICO640_THERMAL_479P",
	"PICO384_THERMAL_384X288",
	"SONY_IMX327_SUBLVDS_2M_30FPS_12BIT",
	"SONY_IMX307_SUBLVDS_2M_30FPS_12BIT",
	"VIVO_MCS369Q_4M_30FPS_12BIT",
	"VIVO_MM308M2_2M_25FPS_8BIT",
	"NEXTCHIP_N5_2M_25FPS_8BIT",
	"SMS_SC3335_MIPI_3M_30FPS_10BIT",
	"SMS_SC3335_SLAVE_MIPI_3M_30FPS_10BIT",
	"SONY_IMX335_MIPI_5M_30FPS_12BIT",
	"SONY_IMX335_MIPI_4M_30FPS_12BIT",
	"SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT",
	"PIXELPLUS_PR2020_1M_25FPS_8BIT",
	"SONY_IMX385_MIPI_2M_30FPS_12BIT",
	"PIXELPLUS_PR2020_1M_30FPS_8BIT",
	"PIXELPLUS_PR2020_2M_25FPS_8BIT",
	"PIXELPLUS_PR2020_2M_30FPS_8BIT",
	"PIXELPLUS_PR2100_2M_25FPS_8BIT",
	"PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT",
	"PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT",
	"SONY_IMX334_MIPI_8M_30FPS_12BIT",
	"SMS_SC8238_MIPI_8M_30FPS_10BIT",
	"VIVO_MCS369_2M_30FPS_12BIT",
	"SMS_SC4210_MIPI_4M_30FPS_12BIT",
	"SMS_SC200AI_MIPI_2M_30FPS_10BIT",
	"NEXTCHIP_N6_2M_4CH_25FPS_8BIT",
	"NEXTCHIP_N5_1M_2CH_25FPS_8BIT",
	"GCORE_GC2053_MIPI_2M_30FPS_10BIT",
	"GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT",
	"GCORE_GC2093_MIPI_2M_30FPS_10BIT",
	"GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT",
	"GCORE_GC4653_MIPI_4M_30FPS_10BIT",
	"SONY_IMX335_MIPI_5M_60FPS_10BIT",
	"SONY_IMX335_MIPI_4M_60FPS_10BIT",
	"GCORE_GC1054_MIPI_1M_30FPS_10BIT",
	"SONY_IMX327_MIPI_2M_60FPS_12BIT",
	"SONY_IMX347_MIPI_4M_60FPS_12BIT",
	"SONY_IMX307_SUBLVDS_2M_60FPS_12BIT",
	"SONY_IMX307_MIPI_2M_60FPS_12BIT",
	"GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT",
	"SONY_IMX335_MIPI_2M_60FPS_10BIT",
	"SMS_SC035HGS_MIPI_480P_120FPS_12BIT",
	"SMS_SC500AI_MIPI_5M_30FPS_10BIT",
	"SMS_SC500AI_MIPI_4M_30FPS_10BIT",
	"SMS_SC501AI_2L_MIPI_5M_30FPS_10BIT",
	"SMS_SC401AI_MIPI_4M_30FPS_10BIT",
	"SMS_SC035GS_MIPI_480P_120FPS_12BIT",
	"SMS_SC3336_MIPI_3M_30FPS_10BIT",
	"SMS_SC2335_MIPI_2M_30FPS_10BIT",
	"SMS_SC2336_MIPI_2M_30FPS_10BIT",
	"SMS_SC4336_MIPI_4M_30FPS_10BIT",
	/* ------ LINEAR END ------*/

	/* ------ WDR 2TO1 BEGIN ------*/
	"SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1",
#ifdef FPGA_PORTING
	"SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1",
#endif
	"SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1",
	"OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1",
	"OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1",
	"OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT_WDR2TO1",
	"OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT_WDR2TO1",
	"OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1",
	"SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1",
	"SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1",
	"SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1",
	"SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1",
	"SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1",
	"SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1",
	"SMS_SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1",
	"SMS_SC500AI_MIPI_4M_30FPS_10BIT_WDR2TO1",
	/* ------ WDR 2TO1 END ------*/
};

static SAMPLE_INI_CFG_S	stDefIniCfg = {
	.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
	.devNum    = 1,
	.enSnsType = SONY_IMX327_MIPI_2M_30FPS_12BIT,
	.enWDRMode = WDR_MODE_NONE,
	.s32BusId  = 3,
	.s32SnsI2cAddr = -1,
	.MipiDev   = 0xFF,
	.u8UseDualSns = 0,
	.enSns2Type = SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT,
	.s32Sns2BusId = 0,
	.s32Sns2I2cAddr = -1,
	.Sns2MipiDev = 0xFF,
};

/*=== Source section parser handler begin === */
static void parse_source_type(SAMPLE_INI_CFG_S *cfg, const char *value,
				CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	(CVI_VOID) param0;
	(CVI_VOID) param1;
	(CVI_VOID) param2;

	SAMPLE_PRT("source type =  %s\n", value);
	if (strcmp(value, "SOURCE_USER_FE") == 0) {
		cfg->enSource = VI_PIPE_FRAME_SOURCE_USER_FE;
	}
}

static void parse_source_devnum(SAMPLE_INI_CFG_S *cfg, const char *value,
				CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	(CVI_VOID) param0;
	(CVI_VOID) param1;
	(CVI_VOID) param2;

	SAMPLE_PRT("devNum =  %s\n", value);
	if (strcmp(value, "1") == 0) {
		cfg->devNum = 1;
	} else if (strcmp(value, "2") == 0) {
		cfg->devNum = 2;
	} else {
		cfg->devNum = 1;
	}
}
/* === Source section parser handler end === */

/* === Sensor section parser handler begin === */
static int parse_lane_id(CVI_S16 *LaneId, const char *value)
{
	char buf[8];
	int offset = 0, idx = 0, k;

	for (k = 0; k < 30; k++) {
		/* find next ',' */
		if (value[k] == ',' || value[k] == '\0') {
			if (k == offset) {
				SAMPLE_PRT("lane_id parse error, is the format correct?\n");
				return -1;
			}
			memset(buf, 0, sizeof(buf));
			memcpy(buf, &value[offset], k - offset);
			buf[k-offset] = '\0';
			LaneId[idx++] = atoi(buf);
			offset = k + 1;
		}

		if (value[k] == '\0' || idx == 5)
			break;
	}

	if (k == 30) {
		SAMPLE_PRT("lane_id parse error, is the format correct?\n");
		return -1;
	}

	return 0;
}

static int parse_pn_swap(CVI_S8 *PNSwap, const char *value)
{
	char buf[8];
	int offset = 0, idx = 0, k;

	for (k = 0; k < 30; k++) {
		/* find next ',' */
		if (value[k] == ',' || value[k] == '\0') {
			if (k == offset) {
				SAMPLE_PRT("lane_id parse error, is the format correct?\n");
				return -1;
			}
			memset(buf, 0, sizeof(buf));
			memcpy(buf, &value[offset], k - offset);
			buf[k-offset] = '\0';
			PNSwap[idx++] = atoi(buf);
			offset = k + 1;
		}

		if (value[k] == '\0' || idx == 5)
			break;
	}

	if (k == 30) {
		SAMPLE_PRT("lane_id parse error, is the format correct?\n");
		return -1;
	}

	return 0;
}

static void parse_sensor_name(SAMPLE_INI_CFG_S *cfg, const char *value,
				CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
#define NAME_SIZE 20
	CVI_U32 index = param0;
	CVI_U32 i;

	(CVI_VOID) param1;
	(CVI_VOID) param2;
	SAMPLE_PRT("sensor =  %s\n", value);
	char sensorNameEnv[NAME_SIZE];

	snprintf(sensorNameEnv, NAME_SIZE, "SENSORNAME%d", index);
	setenv(sensorNameEnv, value, 1);

	for (i = 0; i < SAMPLE_SNS_TYPE_BUTT; i++) {
		if (strcmp(value, snsr_type_name[i]) == 0) {
			if (index == 0) {
				cfg->enSnsType = i;
				cfg->enWDRMode = (i < SAMPLE_SNS_TYPE_LINEAR_BUTT) ?
					WDR_MODE_NONE : WDR_MODE_2To1_LINE;
			} else if (index == 1) {
				cfg->enSns2Type = i;
				cfg->enSns2WDRMode = (i < SAMPLE_SNS_TYPE_LINEAR_BUTT) ?
					WDR_MODE_NONE : WDR_MODE_2To1_LINE;
			} else {
				/* do nothing. */
			}
			break;
		}
	}
	if (i == SAMPLE_SNS_TYPE_BUTT) {
		if (index == 0) {
			cfg->enSnsType = SONY_IMX327_MIPI_2M_30FPS_12BIT;
			cfg->enWDRMode = WDR_MODE_NONE;
		} else if (index == 1) {
			cfg->enSns2Type = SONY_IMX327_MIPI_2M_30FPS_12BIT;
			cfg->enSns2WDRMode = WDR_MODE_NONE;
			cfg->u8UseDualSns = CVI_TRUE;
		} else {
			/* do nothing. */
		}
	}
}

static void parse_sensor_busid(SAMPLE_INI_CFG_S *cfg, const char *value,
				CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	CVI_U32 index = param0;

	(CVI_VOID) param1;
	(CVI_VOID) param2;
	SAMPLE_PRT("bus_id =  %s\n", value);
	if (index == 0)
		cfg->s32BusId = atoi(value);
	else if (index == 1)
		cfg->s32Sns2BusId = atoi(value);
}

static void parse_sensor_i2caddr(SAMPLE_INI_CFG_S *cfg, const char *value,
					CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	CVI_U32 index = param0;

	(CVI_VOID) param1;
	(CVI_VOID) param2;
	SAMPLE_PRT("sns_i2c_addr =  %s\n", value);
	if (index == 0)
		cfg->s32SnsI2cAddr = atoi(value);
	else if (index == 1)
		cfg->s32Sns2I2cAddr = atoi(value);
}

static void parse_sensor_mipidev(SAMPLE_INI_CFG_S *cfg, const char *value,
					CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	CVI_U32 index = param0;

	(CVI_VOID) param1;
	(CVI_VOID) param2;
	SAMPLE_PRT("mipi_dev =  %s\n", value);
	if (index == 0)
		cfg->MipiDev = atoi(value);
	else if (index == 1)
		cfg->Sns2MipiDev = atoi(value);
}

static void parse_sensor_laneid(SAMPLE_INI_CFG_S *cfg, const char *value,
					CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	CVI_U32 index = param0;

	(CVI_VOID) param1;
	(CVI_VOID) param2;
	SAMPLE_PRT("Lane_id =  %s\n", value);
	if (index == 0)
		parse_lane_id(cfg->as16LaneId, value);
	else if (index == 1)
		parse_lane_id(cfg->as16Sns2LaneId, value);
}

static void parse_sensor_pnswap(SAMPLE_INI_CFG_S *cfg, const char *value,
					CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	CVI_U32 index = param0;

	(CVI_VOID) param1;
	(CVI_VOID) param2;
	SAMPLE_PRT("pn_swap =  %s\n", value);
	if (index == 0)
		parse_pn_swap(cfg->as8PNSwap, value);
	else if (index == 1)
		parse_pn_swap(cfg->as8Sns2PNSwap, value);
}

static void parse_sensor_hwsync(SAMPLE_INI_CFG_S *cfg, const char *value,
				CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	CVI_U32 index = param0;

	(CVI_VOID) param1;
	(CVI_VOID) param2;
	SAMPLE_PRT("hw_sync =  %s\n", value);
	if (index == 0)
		cfg->u8HwSync = atoi(value);
	else if (index == 1)
		cfg->u8Sns2HwSync = atoi(value);
}

static void parse_sensor_mclken(SAMPLE_INI_CFG_S *cfg, const char *value,
				CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	CVI_U32 index = param0;

	(CVI_VOID) param1;
	(CVI_VOID) param2;
	SAMPLE_PRT("mclk_en =  %s\n", value);
	if (index == 0)
		cfg->stMclkAttr.bMclkEn = atoi(value);
	else if (index == 1)
		cfg->stSns2MclkAttr.bMclkEn = atoi(value);
}

static void parse_sensor_mclk(SAMPLE_INI_CFG_S *cfg, const char *value,
				CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	CVI_U32 index = param0;

	(CVI_VOID) param1;
	(CVI_VOID) param2;
	SAMPLE_PRT("mclk =  %s\n", value);
	if (index == 0)
		cfg->stMclkAttr.u8Mclk = atoi(value);
	else if (index == 1)
		cfg->stSns2MclkAttr.u8Mclk = atoi(value);
}

static void parse_sensor_orien(SAMPLE_INI_CFG_S *cfg, const char *value,
				CVI_U32 param0, CVI_U32 param1, CVI_U32 param2)
{
	CVI_U32 index = param0;

	(CVI_VOID) param1;
	(CVI_VOID) param2;
	SAMPLE_PRT("orien =  %s\n", value);
	if (index == 0)
		cfg->u8Orien = atoi(value);
	else if (index == 1)
		cfg->u8Sns2Orien = atoi(value);
}

/* === Sensor section parser handler end === */
typedef CVI_VOID(*parser)(SAMPLE_INI_CFG_S *cfg, const char *value,
		CVI_U32 param0, CVI_U32 param1, CVI_U32 param2);

typedef struct _INI_HDLR_S {
	const char name[16];
	CVI_U32 param0;
	CVI_U32 param1;
	CVI_U32 param2;
	parser pfnJob;
} INI_HDLR_S;

typedef enum _INI_SOURCE_NAME_E {
	INI_SOURCE_TYPE = 0,
	INI_SOURCE_DEVNUM,
	INI_SOURCE_NUM,
} INI_SOURCE_NAME_E;

typedef enum _INI_SENSOR_NAME_E {
	INI_SENSOR_NAME = 0,
	INI_SENSOR_BUSID,
	INI_SENSOR_I2CADDR,
	INI_SENSOR_MIPIDEV,
	INI_SENSOR_LANEID,
	INI_SENSOR_PNSWAP,
	INI_SENSOR_HWSYNC,
	INI_SENSOR_MCLKEN,
	INI_SENSOR_MCLK,
	INI_SENSOR_ORIEN,
	INI_SENSOR_NUM,
} INI_SENSOR_NAME_E;

const INI_HDLR_S stSectionSource[INI_SOURCE_NUM] = {
	[INI_SOURCE_TYPE] = {"type", 0, 0, 0, parse_source_type},
	[INI_SOURCE_DEVNUM] = {"dev_num", 0, 0, 0, parse_source_devnum},
};

const INI_HDLR_S stSectionSensor1[INI_SENSOR_NUM] = {
	[INI_SENSOR_NAME] = {"name", 0, 0, 0, parse_sensor_name},
	[INI_SENSOR_BUSID] = {"bus_id", 0, 0, 0, parse_sensor_busid},
	[INI_SENSOR_I2CADDR] = {"sns_i2c_addr", 0, 0, 0, parse_sensor_i2caddr},
	[INI_SENSOR_MIPIDEV] = {"mipi_dev", 0, 0, 0, parse_sensor_mipidev},
	[INI_SENSOR_LANEID] = {"lane_id", 0, 0, 0, parse_sensor_laneid},
	[INI_SENSOR_PNSWAP] = {"pn_swap", 0, 0, 0, parse_sensor_pnswap},
	[INI_SENSOR_HWSYNC] = {"hw_sync", 0, 0, 0, parse_sensor_hwsync},
	[INI_SENSOR_MCLKEN] = {"mclk_en", 0, 0, 0, parse_sensor_mclken},
	[INI_SENSOR_MCLK] = {"mclk", 0, 0, 0, parse_sensor_mclk},
	[INI_SENSOR_ORIEN] = {"orien", 0, 0, 0, parse_sensor_orien},
};

const INI_HDLR_S stSectionSensor2[INI_SENSOR_NUM] = {
	[INI_SENSOR_NAME] = {"name", 1, 0, 0, parse_sensor_name},
	[INI_SENSOR_BUSID] = {"bus_id", 1, 0, 0, parse_sensor_busid},
	[INI_SENSOR_I2CADDR] = {"sns_i2c_addr", 1, 0, 0, parse_sensor_i2caddr},
	[INI_SENSOR_MIPIDEV] = {"mipi_dev", 1, 0, 0, parse_sensor_mipidev},
	[INI_SENSOR_LANEID] = {"lane_id", 1, 0, 0, parse_sensor_laneid},
	[INI_SENSOR_PNSWAP] = {"pn_swap", 1, 0, 0, parse_sensor_pnswap},
	[INI_SENSOR_HWSYNC] = {"hw_sync", 1, 0, 0, parse_sensor_hwsync},
	[INI_SENSOR_MCLKEN] = {"mclk_en", 1, 0, 0, parse_sensor_mclken},
	[INI_SENSOR_MCLK] = {"mclk", 1, 0, 0, parse_sensor_mclk},
	[INI_SENSOR_ORIEN] = {"orien", 1, 0, 0, parse_sensor_orien},
};

static int parse_handler(void *user, const char *section, const char *name, const char *value)
{
	SAMPLE_INI_CFG_S *cfg = (SAMPLE_INI_CFG_S *)user;
	const INI_HDLR_S *hdler;
	int i, size;

	if (strcmp(section, "source") == 0) {
		hdler = stSectionSource;
		size = INI_SOURCE_NUM;
	} else if (strcmp(section, "sensor") == 0) {
		hdler = stSectionSensor1;
		size = INI_SENSOR_NUM;
	} else if (strcmp(section, "sensor2") == 0) {
		hdler = stSectionSensor2;
		size = INI_SENSOR_NUM;
	} else {
		/* unknown section/name */
		return 1;
	}
	for (i = 0; i < size; i++) {
		if (strcmp(name, hdler[i].name) == 0) {
			hdler[i].pfnJob(cfg, value, hdler[i].param0,
					hdler[i].param1, hdler[i].param2);
			break;
		}
	}

	return 1;
}

CVI_S32 SAMPLE_COMM_VI_SetIniPath(const CVI_CHAR *iniPath)
{
	int ret;

	if (iniPath == NULL) {
		SAMPLE_PRT("%s: null ptr\n", __func__);
		ret = CVI_FAILURE;
	} else if (strlen(iniPath) >= SNSCFGPATH_SIZE) {
		SAMPLE_PRT("%s: SNSCFGPATH_SIZE is too small\n", __func__);
		ret = CVI_FAILURE;
	} else {
		strncpy(g_snsCfgPath, iniPath, SNSCFGPATH_SIZE);
		ret = CVI_SUCCESS;
	}

	return ret;
}

CVI_S32 SAMPLE_COMM_VI_ParseIni(SAMPLE_INI_CFG_S *pstIniCfg)
{
	int ret;
#define INI_FILE_PATH	"/root/configs/sensor_cfg.ini"
#define INI_DEF_PATH	"/root/firmware/sensor/sensor_cfg.ini"

	memcpy(pstIniCfg, &stDefIniCfg, sizeof(*pstIniCfg));
	if (g_snsCfgPath[0] != 0) {
		SAMPLE_PRT("Parse %s\n", g_snsCfgPath);
		ret = ini_parse(g_snsCfgPath, parse_handler, pstIniCfg);
		if (ret >= 0) {
			return 1;
		}
		if (ret != -1) {
			SAMPLE_PRT("Parse %s incomplete, use default cfg\n", INI_FILE_PATH);
			return 0;
		}

		SAMPLE_PRT("%s Not Found\n", g_snsCfgPath);
	}
	SAMPLE_PRT("Parse %s\n", INI_FILE_PATH);
	ret = ini_parse(INI_FILE_PATH, parse_handler, pstIniCfg);
	if (ret >= 0) {
		return 1;
	}
	if (ret != -1) {
		SAMPLE_PRT("Parse %s incomplete, use default cfg\n", INI_FILE_PATH);
		return 0;
	}
	SAMPLE_PRT("%s Not Found\n", INI_FILE_PATH);
	SAMPLE_PRT("Parse %s\n", INI_DEF_PATH);

	ret = ini_parse(INI_DEF_PATH, parse_handler, pstIniCfg);
	if (ret < 0) {
		if (ret == -1) {
			SAMPLE_PRT("%s not exist, use default cfg\n", INI_DEF_PATH);
		} else {
			SAMPLE_PRT("Parse %s incomplete, use default cfg\n", INI_DEF_PATH);
		}

		return 0;
	}

	return 1;
}

/* Helper API to fill the stViConfig according to the pstIniCfg. */
CVI_S32 SAMPLE_COMM_VI_IniToViCfg(SAMPLE_INI_CFG_S *pstIniCfg, SAMPLE_VI_CONFIG_S *pstViConfig)
{
	DYNAMIC_RANGE_E    enDynamicRange	= DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat		= PIXEL_FORMAT_YUV_PLANAR_420;
	VIDEO_FORMAT_E	   enVideoFormat	= VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode	= COMPRESS_MODE_TILE;
	VI_VPSS_MODE_E	   enMastPipeMode	= VI_OFFLINE_VPSS_OFFLINE;
	CVI_S32 s32WorkSnsId = 0;

	if (!pstIniCfg) {
		SAMPLE_PRT("%s: null ptr\n", __func__);
		return CVI_FAILURE;
	}

	if (!pstViConfig) {
		SAMPLE_PRT("%s: null ptr\n", __func__);
		return CVI_FAILURE;
	}

	SAMPLE_COMM_VI_GetSensorInfo(pstViConfig);

	for (; s32WorkSnsId < pstIniCfg->devNum; s32WorkSnsId++) {
		pstViConfig->s32WorkingViNum					 = 1 + s32WorkSnsId;
		pstViConfig->as32WorkingViId[s32WorkSnsId]			 = s32WorkSnsId;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.enSnsType		 =
			(s32WorkSnsId == 0) ? pstIniCfg->enSnsType : pstIniCfg->enSns2Type;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.MipiDev		 =
			(s32WorkSnsId == 0) ? pstIniCfg->MipiDev : pstIniCfg->Sns2MipiDev;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.stMclkAttr.bMclkEn	 =
			(s32WorkSnsId == 0) ? pstIniCfg->stMclkAttr.bMclkEn : pstIniCfg->stSns2MclkAttr.bMclkEn;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.stMclkAttr.u8Mclk	 =
			(s32WorkSnsId == 0) ? pstIniCfg->stMclkAttr.u8Mclk : pstIniCfg->stSns2MclkAttr.u8Mclk;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.s32BusId		 =
			(s32WorkSnsId == 0) ? pstIniCfg->s32BusId : pstIniCfg->s32Sns2BusId;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.s32SnsI2cAddr	 =
			(s32WorkSnsId == 0) ? pstIniCfg->s32SnsI2cAddr : pstIniCfg->s32Sns2I2cAddr;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.u8HwSync		 =
			(s32WorkSnsId == 0) ? pstIniCfg->u8HwSync : pstIniCfg->u8Sns2HwSync;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.u8Orien		 =
			(s32WorkSnsId == 0) ? pstIniCfg->u8Orien : pstIniCfg->u8Sns2Orien;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[0]	 =
			(s32WorkSnsId == 0) ? pstIniCfg->as16LaneId[0] : pstIniCfg->as16Sns2LaneId[0];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[1]	 =
			(s32WorkSnsId == 0) ? pstIniCfg->as16LaneId[1] : pstIniCfg->as16Sns2LaneId[1];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[2]	 =
			(s32WorkSnsId == 0) ? pstIniCfg->as16LaneId[2] : pstIniCfg->as16Sns2LaneId[2];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[3]	 =
			(s32WorkSnsId == 0) ? pstIniCfg->as16LaneId[3] : pstIniCfg->as16Sns2LaneId[3];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[4]	 =
			(s32WorkSnsId == 0) ? pstIniCfg->as16LaneId[4] : pstIniCfg->as16Sns2LaneId[4];

		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[0]	=
			(s32WorkSnsId == 0) ? pstIniCfg->as8PNSwap[0] : pstIniCfg->as8Sns2PNSwap[0];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[1]	=
			(s32WorkSnsId == 0) ? pstIniCfg->as8PNSwap[1] : pstIniCfg->as8Sns2PNSwap[1];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[2]	=
			(s32WorkSnsId == 0) ? pstIniCfg->as8PNSwap[2] : pstIniCfg->as8Sns2PNSwap[2];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[3]	=
			(s32WorkSnsId == 0) ? pstIniCfg->as8PNSwap[3] : pstIniCfg->as8Sns2PNSwap[3];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[4]	=
			(s32WorkSnsId == 0) ? pstIniCfg->as8PNSwap[4] : pstIniCfg->as8Sns2PNSwap[4];

		pstViConfig->astViInfo[s32WorkSnsId].stDevInfo.ViDev		 = s32WorkSnsId;
		pstViConfig->astViInfo[s32WorkSnsId].stDevInfo.enWDRMode		 =
			(s32WorkSnsId == 0) ? pstIniCfg->enWDRMode : pstIniCfg->enSns2WDRMode;

		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]		 = s32WorkSnsId;
		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]		 = -1;
		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]		 = -1;
		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]		 = -1;

		pstViConfig->astViInfo[s32WorkSnsId].stChnInfo.ViChn		 = s32WorkSnsId;
		pstViConfig->astViInfo[s32WorkSnsId].stChnInfo.enPixFormat	 = enPixFormat;
		pstViConfig->astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		pstViConfig->astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat	 = enVideoFormat;
		pstViConfig->astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_DefaultConfig(CVI_VOID)
{
	SAMPLE_INI_CFG_S stIniCfg = {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType = SONY_IMX327_MIPI_2M_30FPS_12BIT,
		.enWDRMode = WDR_MODE_NONE,
		.s32BusId  = 3,
		.s32SnsI2cAddr = -1,
		.MipiDev   = 0xFF,
		.u8UseDualSns = 0,
		.enSns2Type = SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT,
		.s32Sns2BusId = 0,
		.s32Sns2I2cAddr = -1,
		.Sns2MipiDev = 0xFF,
	};
	SAMPLE_VI_CONFIG_S stViConfig;
	PIC_SIZE_E enPicSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;

	// Get config from ini if found.
	if (SAMPLE_COMM_VI_ParseIni(&stIniCfg)) {
		SAMPLE_PRT("Parse complete\n");
	}

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	if (s32Ret != CVI_SUCCESS)
		return s32Ret;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType, &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init modules
	 ************************************************/
	s32Ret = SAMPLE_PLAT_SYS_INIT(stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sys init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	if (stIniCfg.enSource == VI_PIPE_FRAME_SOURCE_DEV) {
		s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_CHAR *SAMPLE_COMM_VI_GetSnsrTypeName(void)
{
	return (CVI_CHAR *)snsr_type_name;
}

