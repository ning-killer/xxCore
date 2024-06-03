#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>
#include <pthread.h>
#include "ts_common.h"
#include "sample_comm.h"
#include "loadbmp.h"

#define OverlayMinHandle    0
#define OverlayExMinHandle 20
#define CoverMinHandle     40
#define CoverExMinHandle   60
#define MosaicMinHandle    80
#define MosaicExMinHandle    100

TS_CHAR * Path_BMP = TS_NULL;

TS_S32 REGION_MST_LoadBmp(const char *filename, BITMAP_S *pstBitmap, TS_BOOL bFil, TS_U32 u16FilColor,
	PIXEL_FORMAT_E enPixelFormat)
{
	OSD_SURFACE_S Surface;
	OSD_BITMAPFILEHEADER bmpFileHeader;
	OSD_BITMAPINFO bmpInfo;
	TS_S32 s32BytesPerPix = 2;
	TS_U8 *pu8Data;
	TS_S32 R_Value;
	TS_S32 G_Value;
	TS_S32 B_Value;
	TS_S32 Gr_Value;
	TS_U8  Value_tmp;
	TS_U8  Value;
	TS_S32 s32Width;

	if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
		printf("GetBmpInfo err!\n");
		return TS_FAILURE;
	}

	if (enPixelFormat == PIXEL_FORMAT_ARGB_4444) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_1555 || enPixelFormat == PIXEL_FORMAT_ARGB_2BPP) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
		s32BytesPerPix = 4;
	} else {
		printf("enPixelFormat err %d\n", enPixelFormat);
		return TS_FAILURE;
	}

	pstBitmap->pData = malloc(s32BytesPerPix * (bmpInfo.bmiHeader.biWidth) * (bmpInfo.bmiHeader.biHeight));

	if (NULL == pstBitmap->pData)	{
		printf("malloc osd memroy err!\n");
		return TS_FAILURE;
	}

	CreateSurfaceByBitMap(filename, &Surface, (TS_U8 *)(pstBitmap->pData));

	pstBitmap->u32Width = Surface.u16Width;
	pstBitmap->u32Height = Surface.u16Height;
	pstBitmap->enPixelFormat = enPixelFormat;

	int i, j, k;
	TS_U8 *pu8Temp;

	if (PIXEL_FORMAT_ARGB_2BPP == enPixelFormat) {
		s32Width = DIV_UP(bmpInfo.bmiHeader.biWidth, 4);
		pu8Data = malloc((s32Width) * (bmpInfo.bmiHeader.biHeight));
		if (NULL == pu8Data) {
			printf("malloc osd memroy err!\n");
			return TS_FAILURE;
		}
	}
	if (PIXEL_FORMAT_ARGB_2BPP != enPixelFormat) {
		TS_U16 *pu16Temp;

		pu16Temp = (TS_U16 *)pstBitmap->pData;

		if (bFil) {
			for (i = 0; i < pstBitmap->u32Height; i++) {
				for (j = 0; j < pstBitmap->u32Width; j++) {
					if (u16FilColor == *pu16Temp)
						*pu16Temp &= 0x7FFF;

					pu16Temp++;
				}
			}
		}
	} else {
		TS_U16 *pu16Temp;

		pu16Temp = (TS_U16 *)pstBitmap->pData;
		pu8Temp = (TS_U8 *)pu8Data;
		for (i = 0; i < pstBitmap->u32Height; i++) {
			for (j = 0; j < pstBitmap->u32Width/4; j++) {
				Value = 0;
				for (k = j; k < j + 4; k++) {
					B_Value = *pu16Temp & 0x001F;
					G_Value = *pu16Temp >> 5 & 0x001F;
					R_Value = *pu16Temp >> 10 & 0x001F;
					pu16Temp++;
					Gr_Value = (R_Value * 299 + G_Value * 587 + B_Value * 144 + 500) / 1000;
					if (Gr_Value > 16)
						Value_tmp = 0x01;
					else
						Value_tmp = 0x00;
					Value = (Value << 2) + Value_tmp;
				}
				*pu8Temp = Value;
				pu8Temp++;
			}
		}
		free(pstBitmap->pData);
		pstBitmap->pData = pu8Data;
	}

	return TS_SUCCESS;
}

TS_S32 REGION_MST_UpdateCanvas(const char *filename, BITMAP_S *pstBitmap, TS_BOOL bFil,
				TS_U32 u16FilColor, SIZE_S *pstSize, TS_U32 u32Stride, PIXEL_FORMAT_E enPixelFmt)
{
	OSD_SURFACE_S Surface;
	OSD_BITMAPFILEHEADER bmpFileHeader;
	OSD_BITMAPINFO bmpInfo;

	if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
		printf("GetBmpInfo err!\n");
		return TS_FAILURE;
	}

	if (PIXEL_FORMAT_ARGB_1555 == enPixelFmt)
		Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
	else if (PIXEL_FORMAT_ARGB_4444 == enPixelFmt)
		Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
	else if (PIXEL_FORMAT_ARGB_8888 == enPixelFmt)
		Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
	else {
		printf("Pixel format is not support!\n");
		return TS_FAILURE;
	}

	if (NULL == pstBitmap->pData)	{
		printf("malloc osd memroy err!\n");
		return TS_FAILURE;
	}

	CreateSurfaceByCanvas(filename, &Surface, (TS_U8 *)(pstBitmap->pData), pstSize->u32Width,
							pstSize->u32Height, u32Stride);

	pstBitmap->u32Width = Surface.u16Width;
	pstBitmap->u32Height = Surface.u16Height;

	if (PIXEL_FORMAT_ARGB_1555 == enPixelFmt)
		pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_1555;
	else if (PIXEL_FORMAT_ARGB_4444 == enPixelFmt)
		pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_4444;
	else if (PIXEL_FORMAT_ARGB_8888 == enPixelFmt)
		pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_8888;

	int i, j;
	TS_U16 *pu16Temp;

	pu16Temp = (TS_U16 *)pstBitmap->pData;
	if (bFil)  {
		for (i = 0; i < pstBitmap->u32Height; i++) {
			for (j = 0; j < pstBitmap->u32Width; j++) {
				if (u16FilColor == *pu16Temp)
					*pu16Temp &= 0x7FFF;

				pu16Temp++;
			}
		}

	}

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_REGION_GetMinHandle(RGN_TYPE_E enType)
{
	TS_S32 MinHandle;

	switch (enType) {
	case OVERLAY_RGN:
	    MinHandle = OverlayMinHandle;
	break;
	case OVERLAYEX_RGN:
	    MinHandle = OverlayExMinHandle;
	break;
	case COVER_RGN:
	    MinHandle = CoverMinHandle;
	break;
	case COVEREX_RGN:
	    MinHandle = CoverExMinHandle;
	break;
	case MOSAIC_RGN:
	    MinHandle = MosaicMinHandle;
	break;
	case MOSAICEX_RGN:
	    MinHandle = MosaicExMinHandle;
	break;
	default:
	    MinHandle = -1;
	break;
	}
	return MinHandle;
}

TS_S32 SAMPLE_REGION_CreateOverLay(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = OVERLAY_RGN;
	stRegion.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_ARGB_1555;
	stRegion.unAttr.stOverlay.stSize.u32Height = 200;
	stRegion.unAttr.stOverlay.stSize.u32Width  = 200;
	stRegion.unAttr.stOverlay.u32BgColor = 0x00ff00ff;
	stRegion.unAttr.stOverlay.u32CanvasNum = 2;
	for (i = OverlayMinHandle; i < HandleNum; i++) {
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_CreateOverLayEx(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = OVERLAYEX_RGN;
	stRegion.unAttr.stOverlayEx.enPixelFmt = PIXEL_FORMAT_ARGB_8888;
	stRegion.unAttr.stOverlayEx.stSize.u32Height = 144;
	stRegion.unAttr.stOverlayEx.stSize.u32Width  = 180;
	stRegion.unAttr.stOverlayEx.u32BgColor = 0x00ff00ff;
	stRegion.unAttr.stOverlayEx.u32CanvasNum = 2;
	for (i = OverlayExMinHandle; i < OverlayExMinHandle + HandleNum; i++)	{
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_CreateCover(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = COVER_RGN;

	for (i = CoverMinHandle; i < CoverMinHandle + HandleNum; i++) {
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_CreateCoverEx(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = COVEREX_RGN;

	for (i = CoverExMinHandle; i < CoverExMinHandle + HandleNum; i++) {
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_CreateMosaic(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = MOSAIC_RGN;

	for (i = MosaicMinHandle; i < MosaicMinHandle + HandleNum; i++) {
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_CreateMosaicEx(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = MOSAICEX_RGN;

	for (i = MosaicExMinHandle; i < MosaicExMinHandle + HandleNum; i++) {
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_Destroy(RGN_HANDLE Handle)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_Destroy(Handle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_Destroy failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_SetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_SetAttr(Handle, pstRegion);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_SetAttr failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_GetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_Create(Handle, pstRegion);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_AttachToChn(RGN_HANDLE Handle, MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_AttachToChn(Handle, pstChn, pstChnAttr);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_AttachToChn failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_DetachFromChn(RGN_HANDLE Handle, MPP_CHN_S *pstChn)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_DetachFromChn(Handle, pstChn);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_DetachFromChn failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_SetDisplayAttr(RGN_HANDLE Handle, MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_SetDisplayAttr(Handle, pstChn, pstChnAttr);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_SetDisplayAttr failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_GetDisplayAttr(RGN_HANDLE Handle, MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_GetDisplayAttr(Handle, pstChn, pstChnAttr);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_GetDisplayAttr failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_SetBitMap(RGN_HANDLE Handle, BITMAP_S *pstBitmap)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_SetBitMap(Handle, pstBitmap);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_SetBitMap failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_GetUpCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_GetCanvasInfo(Handle, pstCanvasInfo);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}


	s32Ret = TS_MPI_RGN_UpdateCanvas(Handle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_Create(TS_S32 HandleNum, RGN_TYPE_E enType)
{
	TS_S32 s32Ret;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return TS_FAILURE;
	}
	if (enType < 0 || enType > 5) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return TS_FAILURE;
	}
	switch (enType) {
	case OVERLAY_RGN:
	     s32Ret = SAMPLE_REGION_CreateOverLay(HandleNum);
	break;
	case OVERLAYEX_RGN:
	     s32Ret = SAMPLE_REGION_CreateOverLayEx(HandleNum);
	break;
	case COVER_RGN:
	     s32Ret = SAMPLE_REGION_CreateCover(HandleNum);
	break;
	case COVEREX_RGN:
	     s32Ret = SAMPLE_REGION_CreateCoverEx(HandleNum);
	break;
	case MOSAIC_RGN:
	     s32Ret = SAMPLE_REGION_CreateMosaic(HandleNum);
    break;
    case MOSAICEX_RGN:
      s32Ret = SAMPLE_REGION_CreateMosaicEx(HandleNum);
	break;
	default:
	break;
	}
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("failed! HandleNum%d,entype:%d!\n", HandleNum, enType);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_Destroy(TS_S32 HandleNum, RGN_TYPE_E enType)
{
	TS_S32 i;
	TS_S32 s32Ret = TS_SUCCESS;
	TS_S32 MinHadle;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return TS_FAILURE;
	}
	if (enType < 0 || enType > 5) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return TS_FAILURE;
	}
	switch (enType) {
	case OVERLAY_RGN:
	    MinHadle  = OverlayMinHandle;
	break;
	case OVERLAYEX_RGN:
	    MinHadle  = OverlayExMinHandle;
	break;
	case COVER_RGN:
	    MinHadle  = CoverMinHandle;
	break;
	case COVEREX_RGN:
	    MinHadle  = CoverExMinHandle;
	break;
	case MOSAIC_RGN:
	    MinHadle  = MosaicMinHandle;
	break;
	case MOSAICEX_RGN:
	    MinHadle  = MosaicExMinHandle;
	break;
	default:
	break;
	}
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		s32Ret = SAMPLE_REGION_Destroy(i);
		if (TS_SUCCESS != s32Ret)
			SAMPLE_PRT("failed!\n");
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_AttachToChn(TS_S32 HandleNum, RGN_TYPE_E enType, MPP_CHN_S *pstMppChn)
{
	TS_S32 i;
	TS_S32 s32Ret;
	TS_S32 MinHadle;
	RGN_CHN_ATTR_S stChnAttr;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return TS_FAILURE;
	}
	if (enType < 0 || enType > 5) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return TS_FAILURE;
	}
	if (TS_NULL == pstMppChn) {
		SAMPLE_PRT("pstMppChn is NULL !\n");
		return TS_FAILURE;
	}
	/*set the chn config*/
	stChnAttr.bShow = TS_TRUE;
	switch (enType)  {
	case OVERLAY_RGN:
	    MinHadle = OverlayMinHandle;

	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = OVERLAY_RGN;

	    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 128;
	    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;

	    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = TS_FALSE;
	    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = TS_TRUE;
	    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 30;

	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 16;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = LESSTHAN_LUM_THRESH;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = TS_FALSE;
	    stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[0] = 0x2abc;
	    stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[1] = 0x7FF0;
	    stChnAttr.unChnAttr.stOverlayChn.enAttachDest = ATTACH_JPEG_MAIN;
	break;
	case OVERLAYEX_RGN:
	    MinHadle = OverlayExMinHandle;
	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = OVERLAYEX_RGN;

	    stChnAttr.unChnAttr.stOverlayExChn.u32BgAlpha = 128;
	    stChnAttr.unChnAttr.stOverlayExChn.u32FgAlpha = 128;
	break;
	case COVER_RGN:
	    MinHadle = CoverMinHandle;

	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = COVER_RGN;
	    stChnAttr.unChnAttr.stCoverChn.enCoverType = AREA_RECT;

	    stChnAttr.unChnAttr.stCoverChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stCoverChn.stRect.u32Width  = 200;

	    stChnAttr.unChnAttr.stCoverChn.u32Color      = 0x0000ffff;

	    stChnAttr.unChnAttr.stCoverChn.enCoordinate = RGN_ABS_COOR;
	break;
	case COVEREX_RGN:
	    MinHadle = CoverExMinHandle;

	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = COVEREX_RGN;
	    stChnAttr.unChnAttr.stCoverExChn.enCoverType = AREA_RECT;

	    stChnAttr.unChnAttr.stCoverExChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stCoverExChn.stRect.u32Width  = 200;

	    stChnAttr.unChnAttr.stCoverExChn.u32Color      = 0x0000ffff;
	break;
	case MOSAIC_RGN:
	    MinHadle = MosaicMinHandle;
	    stChnAttr.enType = MOSAIC_RGN;
	    stChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_32;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width  = 200;
	break;
	case MOSAICEX_RGN:
	    MinHadle = MosaicExMinHandle;
	    stChnAttr.enType = MOSAICEX_RGN;
	    stChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_32;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width  = 200;
	break;
    default:
	break;
	}
	/*attach to Chn*/
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		if (OVERLAY_RGN == enType) {
			stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 20 + 200*(i-OverlayMinHandle);
			stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 20 + 200*(i-OverlayMinHandle);
			stChnAttr.unChnAttr.stOverlayChn.u32Layer = i - OverlayMinHandle;
		}
		if (OVERLAYEX_RGN == enType) {
			stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = 20 + 200*(i - OverlayExMinHandle);
			stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = 20 + 200*(i - OverlayExMinHandle);
			stChnAttr.unChnAttr.stOverlayExChn.u32Layer = i - OverlayExMinHandle;
		}
		if (COVER_RGN == enType)	{
			stChnAttr.unChnAttr.stCoverChn.stRect.s32X = 20 + 200*(i - CoverMinHandle);
			stChnAttr.unChnAttr.stCoverChn.stRect.s32Y = 20 + 200*(i - CoverMinHandle);
			stChnAttr.unChnAttr.stCoverChn.u32Layer = i - CoverMinHandle;
		}
		if (COVEREX_RGN == enType) {
			stChnAttr.unChnAttr.stCoverExChn.stRect.s32X = 400 + 200*(i - CoverExMinHandle);
			stChnAttr.unChnAttr.stCoverExChn.stRect.s32Y = 20 + 200*(i - CoverExMinHandle);
			stChnAttr.unChnAttr.stCoverExChn.u32Layer = i - CoverExMinHandle;
		}
		if (MOSAIC_RGN == enType) {
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = 400 + 200*(i-MosaicMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = 20 + 200*(i-MosaicMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.u32Layer = i-MosaicMinHandle;
		}
		if (MOSAICEX_RGN == enType) {
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = 800 + 200*(i-MosaicExMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = 20 + 200*(i-MosaicExMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.u32Layer = i-MosaicExMinHandle;
		}
		s32Ret = SAMPLE_REGION_AttachToChn(i, pstMppChn, &stChnAttr);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("SAMPLE_REGION_AttachToChn failed!\n");
			break;
		}
	}
	/*detach region from chn */
	if (TS_SUCCESS != s32Ret && i > 0)	{
		i--;
		for (; i >= MinHadle; i--)
			s32Ret = SAMPLE_REGION_DetachFromChn(i, pstMppChn);
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_DetachFrmChn(TS_S32 HandleNum, RGN_TYPE_E enType, MPP_CHN_S *pstMppChn)
{
	TS_S32 i;
	TS_S32 s32Ret = TS_SUCCESS;
	TS_S32 MinHadle;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return TS_FAILURE;
	}
	if (enType < 0 || enType > 5) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return TS_FAILURE;
	}
	if (TS_NULL == pstMppChn) {
		SAMPLE_PRT("pstMppChn is NULL !\n");
		return TS_FAILURE;
	}
	switch (enType)  {
	case OVERLAY_RGN:
	    MinHadle  = OverlayMinHandle;
	break;
	case OVERLAYEX_RGN:
	    MinHadle  = OverlayExMinHandle;
	break;
	case COVER_RGN:
	    MinHadle  = CoverMinHandle;
	break;
	case COVEREX_RGN:
	    MinHadle  = CoverExMinHandle;
	break;
	case MOSAIC_RGN:
	    MinHadle  = MosaicMinHandle;
    break;
    case MOSAICEX_RGN:
        MinHadle  = MosaicExMinHandle;
	break;
	default:
	break;
	}
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		s32Ret = SAMPLE_REGION_DetachFromChn(i, pstMppChn);
		if (TS_SUCCESS != s32Ret)
			SAMPLE_PRT("SAMPLE_REGION_DetachFromChn failed! Handle:%d\n", i);
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle, PIXEL_FORMAT_E enPixelFmt)
{
	TS_S32 s32Ret;
	BITMAP_S stBitmap;

	REGION_MST_LoadBmp(Path_BMP, &stBitmap, TS_FALSE, 0, enPixelFmt);
	s32Ret = SAMPLE_REGION_SetBitMap(Handle, &stBitmap);
	if (s32Ret != TS_SUCCESS)
		SAMPLE_PRT("SAMPLE_REGION_SetBitMap failed!Handle:%d\n", Handle);

	free(stBitmap.pData);
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_GetUpCanvas(RGN_HANDLE Handle)
{
	TS_S32 s32Ret;
	SIZE_S stSize;
	BITMAP_S stBitmap;
	RGN_CANVAS_INFO_S stCanvasInfo;

	s32Ret = TS_MPI_RGN_GetCanvasInfo(Handle, &stCanvasInfo);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}

	stBitmap.pData   = (TS_VOID *)(TS_UL)stCanvasInfo.u64VirtAddr;
	//stBitmap.pData   = (TS_VOID *)(TS_UL)stCanvasInfo.p32VirtAddr;
	stSize.u32Width  = stCanvasInfo.stSize.u32Width;
	stSize.u32Height = stCanvasInfo.stSize.u32Height;
	REGION_MST_UpdateCanvas(Path_BMP, &stBitmap, TS_FALSE, 0, &stSize, stCanvasInfo.u32Stride,
		PIXEL_FORMAT_ARGB_1555);

	s32Ret = TS_MPI_RGN_UpdateCanvas(Handle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}


