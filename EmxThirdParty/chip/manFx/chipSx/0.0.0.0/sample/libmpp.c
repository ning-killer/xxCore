#include "libmpp.h"
#include "sample_comm.h"
#include "sample_comm_asj_osd.h"

extern ovd_osd_t g_ovd_osd;

int MID_OSD_Create(int chn, VIDEO_CFG * VideoCfgInfo,const char *imagePath,const char *fontpath)
{
	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

	if (g_ovd_osd.chn[chn].status == ASJ_CHN_OSD_CREATED) {
		SAMPLE_PRT("Already created chn %d.\n", chn);
		return 0;
	}

	if (g_ovd_osd.chn[chn].have_inited == 0) {
		if (0 != osdInit(chn,imagePath,fontpath)) {
			return -1;
		} else {
			g_ovd_osd.chn[chn].status = ASJ_CHN_OSD_CREATED;
			g_ovd_osd.chn[chn].have_inited = 1;
			return 0;
		}
	} else {
		if (0 != osdRestartInit(chn,imagePath,fontpath)) {
			return -1;
		} else {
			g_ovd_osd.chn[chn].status = ASJ_CHN_OSD_CREATED;
			return 0;
		}
	}
}

int MID_OSD_Destroy(int chn, VIDEO_CFG *VideoCfgInfo)
{
	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

	if (g_ovd_osd.chn[chn].status == ASJ_CHN_OSD_CHAOS) {
		SAMPLE_PRT("Already destroyed chn %d.\n", chn);
		return 0;
	}

	if (0 != osdDeinit(chn)) {
		return -1;
	} else {
		g_ovd_osd.chn[chn].status = ASJ_CHN_OSD_CHAOS;
		return 0;
	}
}

int MID_OSD_Text_Reset(int chn)
{
	int ret = APP_COMM_VENC_Stop_OSD_Text(chn,0);
	if (TS_SUCCESS != ret) {
		SAMPLE_PRT("MID_OSD_Set stop osd test chn %d failed %d !\n", chn, ret);
		return -1;
	}
	return ret;
}

int MID_OSD_Set(int chn, MID_OSD_HIDE_TYPE hide, MID_OSD_TYPE type, videoosd_t *pPara, int id)
{
	int ret = -1;

	if (0 > chn || chn > ASJ_CHN_NUM - 1) {
		SAMPLE_PRT("Invalide chn %d !\n", chn);
		return -1;
	}

	if (g_ovd_osd.chn[chn].status != ASJ_CHN_OSD_CREATED) {
		SAMPLE_PRT("Have not created chn %d !\n", chn);
		return -1;
	}

	if (!pPara) {
		SAMPLE_PRT("pPara NULL !\n");
		return -1;
	}

	/*ret = osdDeinit(chn);
	if (ret != 0) {
		printf("osdDeinit failed %d !\n", ret);
		return -1;
	}*/


	if (MID_OSD_TYPE_CHN_SET == type) {

		ret = APP_COMM_VENC_Stop_OSD_Text(chn,id);
	    if (TS_SUCCESS != ret) {
			SAMPLE_PRT("MID_OSD_Set stop osd test chn %d failed %d !\n", chn, ret);
			return -1;
		}
		g_ovd_osd.chn[chn].text.osdhoralign = pPara->horalign;
		g_ovd_osd.chn[chn].text.osdveralign = pPara->veralign;
		g_ovd_osd.chn[chn].text.osd_count = 8;//1-org
		g_ovd_osd.chn[chn].text.osd_text[0].on = (hide == MID_OSD_HIDE ? 0 : 1);
		g_ovd_osd.chn[chn].text.osd_text[0].mode = 0;
		g_ovd_osd.chn[chn].text.osd_text[0].id = (chn + 1) * ASJ_RGN_HDL_CHNNAME_RATIO + id;
		strcpy(g_ovd_osd.chn[chn].text.osd_text[0].text, pPara->chnname);
		g_ovd_osd.chn[chn].text.osd_text[0].osdfont = OSDFONT_BIG;
		g_ovd_osd.chn[chn].text.osd_text[0].osdpos.x = pPara->x;
		g_ovd_osd.chn[chn].text.osd_text[0].osdpos.y = pPara->y;
		g_ovd_osd.chn[chn].text.osd_text[0].color.red = 127;
		g_ovd_osd.chn[chn].text.osd_text[0].color.green = 127;
		g_ovd_osd.chn[chn].text.osd_text[0].color.blue = 127;
		ret = APP_COMM_VENC_Open_OSD_Text(chn);
	    if (0 != ret) {
			SAMPLE_PRT("MID_OSD_Set open osd test chn %d failed %d !\n", chn, ret);
			return -1;
		}	
	} else if (MID_OSD_TYPE_TIME_SET == type) {
		ret = APP_COMM_VENC_Close_OSD_TimeStamp(chn);
	    if (0 != ret) {
			SAMPLE_PRT("MID_OSD_Set stop osd TimeStamp chn %d failed %d !\n", chn, ret);
			return -1;
		}
		g_ovd_osd.chn[chn].timestamp.osdhoralign = pPara->horalign;
		g_ovd_osd.chn[chn].timestamp.id = (chn + 1) * ASJ_RGN_HDL_TIMESTAMP_RATIO;
		g_ovd_osd.chn[chn].timestamp.on = (hide == MID_OSD_HIDE ? 0 : 1);
		g_ovd_osd.chn[chn].timestamp.osdpos.x = pPara->x;
		g_ovd_osd.chn[chn].timestamp.osdpos.y = pPara->y;
		g_ovd_osd.chn[chn].timestamp.DateMode = pPara->DateMode;
		g_ovd_osd.chn[chn].timestamp.TimeMode = pPara->TimeMode;
		ret = APP_COMM_VENC_Open_OSD_TimeStamp(chn, TS_FALSE, NULL);
	    if (0 != ret) {
			SAMPLE_PRT("MID_OSD_Set open osd TimeStamp chn %d failed %d !\n", chn, ret);
			return -1;
		}	
	} else if (MID_OSD_TYPE_LOGO_SET == type) {
		ret = APP_COMM_VENC_Close_OSD_Logo(chn);
	    if (0 != ret) {
			SAMPLE_PRT("MID_OSD_Set stop osd logo chn %d failed %d !\n", chn, ret);
			return -1;
		}
		g_ovd_osd.chn[chn].logo.osdhoralign = pPara->horalign;
		g_ovd_osd.chn[chn].logo.id = (chn + 1) * ASJ_RGN_HDL_LOGO_RATIO;
		g_ovd_osd.chn[chn].logo.on = (hide == MID_OSD_HIDE ? 0 : 1);
		g_ovd_osd.chn[chn].logo.osdpos.x = pPara->x;
		g_ovd_osd.chn[chn].logo.osdpos.y = pPara->y;
		ret = APP_COMM_VENC_Open_OSD_Logo(chn,NULL);
	    if (0 != ret) {
			SAMPLE_PRT("MID_OSD_Set open osd logo chn %d failed %d !\n", chn, ret);
			return -1;
		}
	} else {
		SAMPLE_PRT("Unknown MID_OSD_TYPE %d !\n", type);
		return -1;
	}
	//return osdRestartInit(chn);
	return 0;
}
