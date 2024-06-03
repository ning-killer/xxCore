#ifndef _MPI_VGS_H_INNER
#define _MPI_VGS_H_INNER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define _SOFT_LINE(fmt...)   \
	do {\
		printf("[%s]-%d: ", __func__, __LINE__);\
		printf(fmt);\
	} while (0)

typedef struct{
   int left;
   int top;
   int right;
   int bottom;
}RECT;

typedef enum
{	
	YUV_WHITE,
	YUV_GREEN,
	YUV_PURPLE,
	YUV_RED,
	YUV_BLUE,
	YUV_DARK_GREEN,
	YUV_YELLOW,
	YUV_LIGHT_BLUE,
	YUV_LIGHT_PURPLE,
	YUV_DARK_BLACK,
	YUV_GRAY,
	YUV_ORANGE,
	YUV_BROWN,
	YUV_DARK_BROWN,
	YUV_PURPLE_RED,
	YUV_MIDNIGHT_BLUE,
	YUV_MEDIUM_BLUE,
	YUV_SKY_BLUE,
	YUV_SPRING_GREEN,
	YUV_SPRING_GOLD,
	YUV_COLOR_MAX,
}enYuvColorIdx;

int YUY_Draw_Line(unsigned char * YuvBuf, int ImgW, int ImgH, int fmt,int SPntX, int SPntY,int EPntX, int EPntY, int Color,int lineWidth);
int YUV_Draw_Rect(unsigned char *YuvBuf, int ImgW, int ImgH, int fmt,RECT* Rect, int Color,int lineWidth);
int YUV_Draw_BodyTrack_Rect(unsigned char *YuvBuf, int ImgW, int ImgH, int fmt,RECT* Rect, int id,int Color,int lineWidth);
int YUY_Draw_num(unsigned char * YuvBuf, int ImgW, int ImgH, int fmt,int SPntX, int SPntY,int id, int Color);

int get_time_ms(char *buff, int len);

#endif

