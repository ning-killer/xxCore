#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>
#include <pthread.h>

#include "sample_comm.h"
#include "loadbmp.h"

#define OverlayMinHandle 0
#define OverlayExMinHandle 20
#define CoverMinHandle 40
#define CoverExMinHandle 60
#define MosaicMinHandle 80

CVI_S32 REGION_MST_LoadBmp(const char *filename, BITMAP_S *pstBitmap, CVI_BOOL bFil, CVI_U32 u16FilColor,
			  PIXEL_FORMAT_E enPixelFormat)
{
	OSD_SURFACE_S Surface;
	OSD_BITMAPFILEHEADER bmpFileHeader;
	OSD_BITMAPINFO bmpInfo;
	CVI_S32 bpp = 2;

	if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
		printf("GetBmpInfo err!\n");
		return CVI_FAILURE;
	}

	if (enPixelFormat == PIXEL_FORMAT_ARGB_4444) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_1555) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
		bpp = 4;
	} else {
		printf("enPixelFormat err %d\n", enPixelFormat);
		return CVI_FAILURE;
	}

	pstBitmap->pData = malloc(bpp * (bmpInfo.bmiHeader.biWidth) * (bmpInfo.bmiHeader.biHeight));
	if (pstBitmap->pData == NULL) {
		printf("malloc osd memroy err!\n");
		return CVI_FAILURE;
	}

	CreateSurfaceByBitMap(filename, &Surface, (CVI_U8 *)(pstBitmap->pData));

	pstBitmap->u32Width = Surface.u16Width;
	pstBitmap->u32Height = Surface.u16Height;
	pstBitmap->enPixelFormat = enPixelFormat;

	if (bFil) {
		CVI_U32 i, j;
		CVI_U16 *pu16Temp;

		pu16Temp = (CVI_U16 *)pstBitmap->pData;
		for (i = 0; i < pstBitmap->u32Height; i++) {
			for (j = 0; j < pstBitmap->u32Width; j++) {
				if (u16FilColor == *pu16Temp) {
					*pu16Temp &= 0x7FFF;
				}

				pu16Temp++;
			}
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 REGION_MST_UpdateCanvas(const char *filename, BITMAP_S *pstBitmap, CVI_BOOL bFil, CVI_U32 u16FilColor,
			       SIZE_S *pstSize, CVI_U32 u32Stride, PIXEL_FORMAT_E enPixelFormat)
{
	OSD_SURFACE_S Surface;
	OSD_BITMAPFILEHEADER bmpFileHeader;
	OSD_BITMAPINFO bmpInfo;

	if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
		printf("GetBmpInfo err!\n");
		return CVI_FAILURE;
	}

	if (enPixelFormat == PIXEL_FORMAT_ARGB_1555) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_4444) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
	} else {
		printf("Pixel format is not support!\n");
		return CVI_FAILURE;
	}

	if (pstBitmap->pData == NULL) {
		printf("malloc osd memroy err!\n");
		return CVI_FAILURE;
	}

	CreateSurfaceByCanvas(filename, &Surface, (CVI_U8 *)(pstBitmap->pData)
			     , pstSize->u32Width, pstSize->u32Height, u32Stride);

	pstBitmap->u32Width = Surface.u16Width;
	pstBitmap->u32Height = Surface.u16Height;
	pstBitmap->enPixelFormat = enPixelFormat;

	// if pixel value match color, make it transparent.
	// Only works for ARGB1555
	if (bFil) {
		CVI_U32 i, j;
		CVI_U16 *pu16Temp;

		pu16Temp = (CVI_U16 *)pstBitmap->pData;
		for (i = 0; i < pstBitmap->u32Height; i++) {
			for (j = 0; j < pstBitmap->u32Width; j++) {
				if (u16FilColor == *pu16Temp)
					*pu16Temp &= 0x7FFF;

				pu16Temp++;
			}
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_REGION_GetMinHandle(RGN_TYPE_E enType)
{
	CVI_S32 MinHandle;

	switch (enType) {
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
	default:
		MinHandle = -1;
		break;
	}
	return MinHandle;
}

CVI_S32 SAMPLE_REGION_CreateOverLayEx(CVI_S32 HandleNum)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = OVERLAYEX_RGN;
	stRegion.unAttr.stOverlayEx.enPixelFormat = PIXEL_FORMAT_ARGB_1555;
	stRegion.unAttr.stOverlayEx.stSize.u32Height = 200;
	stRegion.unAttr.stOverlayEx.stSize.u32Width = 300;
	stRegion.unAttr.stOverlayEx.u32BgColor = 0x00000000; // ARGB1555 transparent
	stRegion.unAttr.stOverlayEx.u32CanvasNum = 2;
	for (i = OverlayExMinHandle; i < OverlayExMinHandle + HandleNum; i++) {
		s32Ret = CVI_RGN_Create(i, &stRegion);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_CreateCover(CVI_S32 HandleNum)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = COVER_RGN;

	for (i = CoverMinHandle; i < CoverMinHandle + HandleNum; i++) {
		s32Ret = CVI_RGN_Create(i, &stRegion);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_CreateCoverEx(CVI_S32 HandleNum)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = COVEREX_RGN;

	for (i = CoverExMinHandle; i < CoverExMinHandle + HandleNum; i++) {
		s32Ret = CVI_RGN_Create(i, &stRegion);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_CreateMosaic(CVI_S32 HandleNum)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = MOSAIC_RGN;

	for (i = MosaicMinHandle; i < MosaicMinHandle + HandleNum; i++) {
		s32Ret = CVI_RGN_Create(i, &stRegion);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_Destroy(RGN_HANDLE Handle)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_Destroy(Handle);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_Destroy failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_AttachToChn(RGN_HANDLE Handle, MMF_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_AttachToChn(Handle, pstChn, pstChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_DetachFromChn(RGN_HANDLE Handle, MMF_CHN_S *pstChn)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_DetachFromChn(Handle, pstChn);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_DetachFromChn failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_SetDisplayAttr(RGN_HANDLE Handle, MMF_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_SetDisplayAttr(Handle, pstChn, pstChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_SetDisplayAttr failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_GetDisplayAttr(RGN_HANDLE Handle, MMF_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_GetDisplayAttr(Handle, pstChn, pstChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_GetDisplayAttr failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_SetBitMap(RGN_HANDLE Handle, BITMAP_S *pstBitmap)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_SetBitMap(Handle, pstBitmap);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_SetBitMap failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_GetUpCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo)
{
	CVI_S32 s32Ret;

	s32Ret = CVI_RGN_GetCanvasInfo(Handle, pstCanvasInfo);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	s32Ret = CVI_RGN_UpdateCanvas(Handle);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_Create(CVI_S32 HandleNum, RGN_TYPE_E enType)
{
	CVI_S32 s32Ret;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return CVI_FAILURE;
	}
	if (enType < OVERLAYEX_RGN || enType >= RGN_BUTT) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return CVI_FAILURE;
	}
	switch (enType) {
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
	default:
		s32Ret = CVI_FAILURE;
		break;
	}
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed! HandleNum%d,entype:%d!\n", HandleNum, enType);
		return CVI_FAILURE;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_Destroy(CVI_S32 HandleNum, RGN_TYPE_E enType)
{
	CVI_S32 i;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 MinHadle;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return CVI_FAILURE;
	}
	if (enType < OVERLAYEX_RGN || enType >= RGN_BUTT) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return CVI_FAILURE;
	}
	switch (enType) {
	case OVERLAYEX_RGN:
		MinHadle = OverlayExMinHandle;
		break;
	case COVER_RGN:
		MinHadle = CoverMinHandle;
		break;
	case COVEREX_RGN:
		MinHadle = CoverExMinHandle;
		break;
	case MOSAIC_RGN:
		MinHadle = MosaicMinHandle;
		break;
	default:
		return CVI_FAILURE;
	}
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		s32Ret = SAMPLE_REGION_Destroy(i);
		if (s32Ret != CVI_SUCCESS)
			SAMPLE_PRT("SAMPLE_COMM_REGION_Destroy failed!\n");
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_AttachToChn(CVI_S32 HandleNum, RGN_TYPE_E enType, MMF_CHN_S *pstChn)
{
	CVI_S32 i;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 MinHadle;
	RGN_CHN_ATTR_S stChnAttr;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return CVI_FAILURE;
	}
	if (enType < OVERLAYEX_RGN || enType >= RGN_BUTT) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return CVI_FAILURE;
	}
	if (pstChn == CVI_NULL) {
		SAMPLE_PRT("pstChn is NULL !\n");
		return CVI_FAILURE;
	}
	/*set the chn config*/
	stChnAttr.bShow = CVI_TRUE;
	switch (enType) {
	case OVERLAYEX_RGN:
		MinHadle = OverlayExMinHandle;
		stChnAttr.bShow = CVI_TRUE;
		stChnAttr.enType = OVERLAYEX_RGN;
		stChnAttr.unChnAttr.stOverlayExChn.stInvertColor.bInvColEn = CVI_FALSE;
		break;
	case COVER_RGN:
		MinHadle = CoverMinHandle;

		stChnAttr.bShow = CVI_TRUE;
		stChnAttr.enType = COVER_RGN;
		stChnAttr.unChnAttr.stCoverChn.enCoverType = AREA_RECT;

		stChnAttr.unChnAttr.stCoverChn.stRect.u32Height = 100;
		stChnAttr.unChnAttr.stCoverChn.stRect.u32Width = 100;

		stChnAttr.unChnAttr.stCoverChn.u32Color = 0x0000ffff;

		stChnAttr.unChnAttr.stCoverChn.enCoordinate = RGN_ABS_COOR;
		break;
	case COVEREX_RGN:
		MinHadle = CoverExMinHandle;

		stChnAttr.bShow = CVI_TRUE;
		stChnAttr.enType = COVEREX_RGN;
		stChnAttr.unChnAttr.stCoverExChn.enCoverType = AREA_RECT;

		stChnAttr.unChnAttr.stCoverExChn.stRect.u32Height = 100;
		stChnAttr.unChnAttr.stCoverExChn.stRect.u32Width = 100;

		stChnAttr.unChnAttr.stCoverExChn.u32Color = 0x0000ffff;
		break;
	case MOSAIC_RGN:
		MinHadle = MosaicMinHandle;
		stChnAttr.enType = MOSAIC_RGN;
		stChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_32;
		stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = 100;
		stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width = 100;
		break;
	default:
		return CVI_FAILURE;
	}
	/*attach to Chn*/
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		if (enType == OVERLAYEX_RGN) {
			stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = 20 + 200 * (i - OverlayExMinHandle);
			stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = 20 + 200 * (i - OverlayExMinHandle);
			stChnAttr.unChnAttr.stOverlayExChn.u32Layer = i - OverlayExMinHandle;
		}
		if (enType == COVER_RGN) {
			stChnAttr.unChnAttr.stCoverChn.stRect.s32X = 20 + 200 * (i - CoverMinHandle);
			stChnAttr.unChnAttr.stCoverChn.stRect.s32Y = 20 + 200 * (i - CoverMinHandle);
			stChnAttr.unChnAttr.stCoverChn.u32Layer = i - CoverMinHandle;
		}
		if (enType == COVEREX_RGN) {
			stChnAttr.unChnAttr.stCoverExChn.stRect.s32X = 20 + 200 * (i - CoverExMinHandle);
			stChnAttr.unChnAttr.stCoverExChn.stRect.s32Y = 20 + 200 * (i - CoverExMinHandle);
			stChnAttr.unChnAttr.stCoverExChn.u32Layer = i - CoverExMinHandle;
		}
		if (enType == MOSAIC_RGN) {
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = 20 + 200 * (i - MosaicMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = 20 + 200 * (i - MosaicMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.u32Layer = i - MosaicMinHandle;
		}
		s32Ret = SAMPLE_REGION_AttachToChn(i, pstChn, &stChnAttr);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("SAMPLE_REGION_AttachToChn failed!\n");
			break;
		}
	}
	/*detach region from chn */
	if (s32Ret != CVI_SUCCESS && i > 0) {
		i--;
		for (; i >= MinHadle; i--)
			s32Ret = SAMPLE_REGION_DetachFromChn(i, pstChn);
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_DetachFrmChn(CVI_S32 HandleNum, RGN_TYPE_E enType, MMF_CHN_S *pstChn)
{
	CVI_S32 i;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 MinHadle;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return CVI_FAILURE;
	}
	if (enType < OVERLAYEX_RGN || enType >= RGN_BUTT) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return CVI_FAILURE;
	}
	if (pstChn == CVI_NULL) {
		SAMPLE_PRT("pstChn is NULL !\n");
		return CVI_FAILURE;
	}
	switch (enType) {
	case OVERLAYEX_RGN:
		MinHadle = OverlayExMinHandle;
		break;
	case COVER_RGN:
		MinHadle = CoverMinHandle;
		break;
	case COVEREX_RGN:
		MinHadle = CoverExMinHandle;
		break;
	case MOSAIC_RGN:
		MinHadle = MosaicMinHandle;
		break;
	default:
		return CVI_FAILURE;
	}
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		s32Ret = SAMPLE_REGION_DetachFromChn(i, pstChn);
		if (s32Ret != CVI_SUCCESS)
			SAMPLE_PRT("SAMPLE_REGION_DetachFromChn failed! Handle:%d\n", i);
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle, const char *filename)
{
	CVI_S32 s32Ret;
	BITMAP_S stBitmap;

	REGION_MST_LoadBmp(filename, &stBitmap, CVI_FALSE, 0, PIXEL_FORMAT_ARGB_1555);
	s32Ret = SAMPLE_REGION_SetBitMap(Handle, &stBitmap);
	if (s32Ret != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_REGION_SetBitMap failed!Handle:%d\n", Handle);
	free(stBitmap.pData);
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_REGION_GetUpCanvas(RGN_HANDLE Handle, const char *filename)
{
	CVI_S32 s32Ret;
	SIZE_S stSize;
	BITMAP_S stBitmap;
	RGN_CANVAS_INFO_S stCanvasInfo;

	s32Ret = CVI_RGN_GetCanvasInfo(Handle, &stCanvasInfo);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	stBitmap.pData = stCanvasInfo.pu8VirtAddr;
	stSize.u32Width = stCanvasInfo.stSize.u32Width;
	stSize.u32Height = stCanvasInfo.stSize.u32Height;
	REGION_MST_UpdateCanvas(filename, &stBitmap, CVI_FALSE, 0, &stSize, stCanvasInfo.u32Stride,
				PIXEL_FORMAT_ARGB_1555);

	s32Ret = CVI_RGN_UpdateCanvas(Handle);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	return s32Ret;
}
