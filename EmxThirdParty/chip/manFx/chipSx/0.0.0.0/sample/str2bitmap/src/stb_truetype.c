/********************************************************************************
 **********Copyright (C), 2022, Beijing Tsing Micro Technology  Co., Ltd.***********
 ********************************************************************************
 * @file     	 : stb_truetype.c
 * @brief   	 : stb_truetype库的使用
 * @author       : Tsing Micro
 * @date         : 2022-12-20
********************************************************************************/
#ifdef __cplusplus
#if __cplusplus
				extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "elog.h"
#include "bitmap.h"
#include "common.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


#define MY_UTF8_INVALID 0xFFFD //internal invalid utf8 rune
#define MY_UTF8_SIZE 4 //describes the number of bytes a glyph consists of*/
#define MY_LEN(a) (sizeof(a)/sizeof(a)[0])
#define MY_BETWEEN(x, a, b) ((a) <= (x) && (x) < (b))
static const uint8_t MY_UTF8BYTE[MY_UTF8_SIZE + 1] = { 0x80, 0, 0xC0, 0xE0, 0xF0 };
static const uint8_t MY_UTF8MASK[MY_UTF8_SIZE + 1] = { 0xC0, 0x80, 0xE0, 0xF0, 0xF8 };
static const uint32_t MY_UTF8MIN[MY_UTF8_SIZE + 1] = { 0, 0, 0x80, 0x800, 0x10000 };
static const uint32_t MY_UTF8MAX[MY_UTF8_SIZE + 1] = { 0x10FFFF, 0x7F, 0x7FF, 0xFFFF, 0x10FFFF };

stbtt_fontinfo fontinfo;
static int fontInitCompleted = 0;
static char font_path[256] = {};
int getUtf8charByteNum(unsigned char ch)
{
    int byteNum = 0;
    if (ch >= 0xFC && ch < 0xFE)
        byteNum = 6;
    else if (ch >= 0xF8)
        byteNum = 5;
    else if (ch >= 0xF0)
        byteNum = 4;
    else if (ch >= 0xE0)
        byteNum = 3;
    else if (ch >= 0xC0)
        byteNum = 2;
    else if (0 == (ch & 0x80))
        byteNum = 1;
    return byteNum;
}

int getUtf8Length(char *str)
{
    int clen = 0;
    int len = 0;
    int byteNum = 0;
    unsigned char ch;
    char *ptr = str;
    if (NULL == str)
        return 0;
    clen = strlen(str);
    while (*ptr != '\0' && len < clen)
    {
        ch = (unsigned char)*ptr;
        if (0 == (byteNum = getUtf8charByteNum(ch)))
            return 0;
        ptr += byteNum;
        len++;
    }
    return len;
}

/*统计utf8字符串里汉字和英文的数量*/
void countOfContents(const unsigned char * text, int *chineseNum,int *asciiNum)
{
    *chineseNum = 0;
    *asciiNum = 0;

    unsigned char ch;

    while( *text )
    {
        ch = (unsigned char)(*text);
        if(ch >= 0xe0 )
        {
            (*chineseNum)++;
            text+=3;
        }
        else
        {
            (*asciiNum)++;
            text++;
        }
    }
}

static int utf8Validate(uint32_t* u, int i)
{
    if (!u)
    {
        return 0;
    }
    if (!MY_BETWEEN(*u, MY_UTF8MIN[i], MY_UTF8MAX[i]) || MY_BETWEEN(*u, 0xD800, 0xDFFF))
    {
        *u = MY_UTF8_INVALID;
    }
    for (i = 1; *u > MY_UTF8MAX[i]; ++i);
    return i;
}

static uint32_t utf8DecodeByte(char c, int* i)
{
    if (!i)
    {
        return 0;
    }
    for (*i = 0; *i < (int)MY_LEN(MY_UTF8MASK); ++(*i))
    {
        if (((uint8_t)c & MY_UTF8MASK[*i]) == MY_UTF8BYTE[*i])
        {
            return (uint8_t)(c & ~MY_UTF8MASK[*i]);
        }
    }
    return 0;
}
static int utf8Decode(char* c, uint32_t* u, int clen)
{
    int i, j, len, type = 0;
    uint32_t udecoded;

    if (!c || !u || !clen)
    {
        return 0;
    }
    *u = MY_UTF8_INVALID;
    udecoded = utf8DecodeByte(c[0], &len);
    if (!MY_BETWEEN(len, 1, MY_UTF8_SIZE))
    {
        return 1;
    }
    for (i = 1, j = 1; i < clen && j < len; ++i, ++j)
    {
        udecoded = (udecoded << 6) | utf8DecodeByte(c[i], &type);
        if (type != 0)
        {
            return j;
        }
    }
    if (j < len)
    {
        return 0;
    }
    *u = udecoded;
    utf8Validate(u, len);
    return len;
}

#if 0
static char utf8EncodeByte(uint32_t u, int i)
{
    return (char)((MY_UTF8BYTE[i]) | ((uint8_t)u & ~MY_UTF8MASK[i]));
}

static int utf8Encode(uint32_t u, char* c, int clen)
{
    int len, i;
    len = utf8Validate(&u, 0);
    if (clen < len || !len || len > MY_UTF8_SIZE)
    {
        return 0;
    }
    for (i = len - 1; i != 0; --i)
    {
        c[i] = utf8EncodeByte(u, 0);
        u >>= 6;
    }
    c[0] = utf8EncodeByte(u, len);
    return len;
}

static int utf8Len(const char* str, int len)
{
    const char* text;
    int glyphs = 0;
    int text_len;
    int glyph_len;
    int src_len = 0;
    uint32_t unicode;
    if (!str || !len)
    {
        return 0;
    }
    text = str;
    text_len = len;
    glyph_len = utf8Decode(text, &unicode, text_len);
    while (glyph_len && src_len < len)
    {
        glyphs++;
        src_len += glyph_len;
        glyph_len = utf8Decode(text + src_len, &unicode, text_len - src_len);
    }
    return glyphs;
}

static const char* utf8At(const char* buffer, int length, int index, uint32_t* unicode, int* len)
{
    int i = 0;
    int src_len = 0;
    int glyph_len = 0;
    const char* text;
    int text_len;

    if (!buffer || !unicode || !len)
    {
        return 0;
    }
    if (index < 0)
    {
        *unicode = MY_UTF8_INVALID;
        *len = 0;
        return 0;
    }
    text = buffer;
    text_len = length;
    glyph_len = utf8Decode(text, unicode, text_len);
    while (glyph_len)
    {
        if (i == index)
        {
            *len = glyph_len;
            break;
        }
        i++;
        src_len = src_len + glyph_len;
        glyph_len = utf8Decode(text + src_len, unicode, text_len - src_len);
    }
    if (i != index)
    {
        return 0;
    }
    return buffer + src_len;
}
#endif

static pthread_mutex_t mutex_lock;
int fontinfoInit(const char *fontpath)
{
    int iRet = 0;
    long int size = 0;
    unsigned char* fontBuffer = NULL;

    FILE *fontFile = fopen(fontpath, "rb");//fontpath  "/tmp/default.ttf"
    if (fontFile == NULL)
    {
        log_e("Can not open font file!");
        return -1;
    }

    fseek(fontFile, 0, SEEK_END);
    size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    fontBuffer = (unsigned char*)calloc(size, sizeof(unsigned char));
    if(NULL == fontBuffer)
    {
        log_e("fontBuffer calloc failed");
        return -1;
    }
    log_d("size is %ld",size);
    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    iRet = stbtt_InitFont(&fontinfo, fontBuffer, 0);
    if (0 == iRet)
    {
        log_e("stb init font failed");
    }
    //free(fontBuffer);
	iRet = pthread_mutex_init(&mutex_lock, NULL);
	if (iRet != 0) {
		log_e("pthread_mutex_init mutex_lock failed !\n");
		return -1;
	}

    fontInitCompleted = 1;

    return iRet;
}

unsigned char * textToBitmap(unsigned char *text,float scale,int bitmap_w,int bitmap_h, unsigned char* inputBitmap)
{
	unsigned char* bitmap = NULL;

    //创建位图
	if(NULL == inputBitmap) {
		bitmap = (unsigned char*)calloc(bitmap_w * bitmap_h, sizeof(unsigned char));
	} else {
		bitmap = inputBitmap;
	}

    int ascent = 0;
    int descent = 0;
    int lineGap = 0;
    //取得竖直方向上的度量
    stbtt_GetFontVMetrics(&fontinfo, &ascent, &descent, &lineGap);
    //根据缩放调整字高
    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);
  //  log_d("ascent:%d,descent:%d",ascent,descent);
    //位图的x
    int x = 0;
    //循环加载text中每个字符
    int text_len = strlen((char *)text);
    int curr_len = 0;
    int next_len = 0;
    uint32_t word = 0;
    uint32_t next_word = 0;
    ////////////////////////////////////////
    // 获取水平方向上的度量
    // advanceWidth：字宽；
    // leftSideBearing：左侧位置；
    int advanceWidth = 0;
    int leftSideBearing = 0;
    int c_x1 = 0;
    int c_y1 = 0;
    int c_x2 = 0;
    int c_y2 = 0;
    //计算位图的y (不同字符的高度不同）
    int y = 0;
    //渲染字符
    int byteOffset = 0;
    //调整字距
    int kern = 0;
    next_len = utf8Decode((char *)(text + curr_len), &word, text_len - curr_len);

    //log_d("text_len:%u",text_len);
    while (next_len)
    {
        curr_len += next_len;
        // 获取水平方向上的度量
        advanceWidth = leftSideBearing = 0;
        stbtt_GetCodepointHMetrics(&fontinfo, word, &advanceWidth, &leftSideBearing);
        //printf("advanceWidth:%d,leftSideBearing:%d\n",advanceWidth,leftSideBearing);
        //获取字符的边框（边界）
        c_x1 = c_y1 = c_x2 = c_y2 = 0;
        stbtt_GetCodepointBitmapBox(&fontinfo, word, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
        //计算位图的y (不同字符的高度不同）
        y = ascent + c_y1;
        //渲染字符
        byteOffset = x + roundf(leftSideBearing * scale) + (y * bitmap_w);
        stbtt_MakeCodepointBitmap(&fontinfo, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word);
        //调整x
        x += roundf(advanceWidth * scale);

        next_len = utf8Decode((char *)(text + curr_len), &next_word, text_len - curr_len);
        if (next_len > 0)
        {
            //调整字距
            kern = stbtt_GetCodepointKernAdvance(&fontinfo, word, next_word);
            x += roundf(kern * scale);
            word = next_word;
        }
    }

    return bitmap;
}

int getBitmapTrueWidth(unsigned char *text, float pixels)
{
    //int text_num = getUtf8Length((char *)text);
    //计算字体缩放
    float scale = stbtt_ScaleForPixelHeight(&fontinfo, pixels); //scale = pixels / (ascent - descent)
    int x = 0;
    int text_len = strlen((char *)text);
    int curr_len = 0;
    int next_len = 0;
    uint32_t word = 0;
    uint32_t next_word = 0;
    int advanceWidth = 0;
    int leftSideBearing = 0;
    //int c_x1 = 0;
    //int c_y1 = 0;
    //int c_x2 = 0;
    //int c_y2 = 0;
    int kern = 0;
    next_len = utf8Decode((char *)(text + curr_len), &word, text_len - curr_len);
    while (next_len)
    {
        curr_len += next_len;
        advanceWidth = leftSideBearing = 0;
        stbtt_GetCodepointHMetrics(&fontinfo, word, &advanceWidth, &leftSideBearing);
        x += roundf(advanceWidth * scale);
        next_len = utf8Decode((char *)(text + curr_len), &next_word, text_len - curr_len);
        if (next_len > 0)
        {
            kern = stbtt_GetCodepointKernAdvance(&fontinfo, word, next_word);
            x += roundf(kern * scale);
            word = next_word;
        }
    }
    return x;
}

unsigned char * stb_treutypeStart(unsigned char *text,int *width,int *height)
{
    int chineseNum =0;
    int asciiNum = 0;

	if(0 == fontInitCompleted){
		fontinfoInit(font_path);
	}

    countOfContents(text,&chineseNum,&asciiNum);
    log_d("chineseNum:%d,asciiNum:%d",chineseNum,asciiNum);


    float pixels = 24.0;
    //计算字体缩放
    float scale = stbtt_ScaleForPixelHeight(&fontinfo, pixels); //scale = pixels / (ascent - descent)
    log_d("scale:%f",scale);

    int bitmap_w = roundf( (chineseNum*2+asciiNum)*128*scale); //位图的宽
    log_d("bitmap_w:%d",bitmap_w);

    int bitmap_h = 24; //位图的高

    *width = bitmap_w;
    *height = bitmap_h;

    unsigned char* bitmap = textToBitmap(text,scale,bitmap_w,bitmap_h, NULL);

    unsigned char *argbBuf = (unsigned char*)malloc(bitmap_w*bitmap_h*2);
	if(NULL == argbBuf)
	{
		log_e("malloc error");
		free(bitmap);
		return NULL;
	}

    bitmapToArgb1555(argbBuf, bitmap_w*bitmap_h*2, bitmap,bitmap_w,bitmap_h);

	free(bitmap);

	return (argbBuf);
}

unsigned char * getArgbDataAfterHandleStr(unsigned char *text,int *width,int *height, float pixels ,int bitmap_h,  unsigned char* inputBitmap,const char *fontPath)
{
   // int chineseNum =0;
   // int asciiNum = 0;
	unsigned char *argbBuf = NULL;
	unsigned char* bitmap = NULL;
    snprintf(font_path, sizeof(font_path), "%s", fontPath);//fontPath
	if(0 == fontInitCompleted){
		fontinfoInit(fontPath);
	}
	pthread_mutex_lock(&mutex_lock);
   // countOfContents(text,&chineseNum,&asciiNum);
   // log_d("chineseNum:%d,asciiNum:%d",chineseNum,asciiNum);

    //计算字体缩放
    float scale = stbtt_ScaleForPixelHeight(&fontinfo, pixels); //scale = pixels / (ascent - descent)
   // log_d("scale:%f",scale);

  ///  int bitmap_w = roundf( (chineseNum*2+asciiNum)*128*scale); //位图的宽
  //  log_d("bitmap_w:%d",bitmap_w);

   // int bitmap_h = 24; //位图的高
  //int bitmap_w_one = roundf(128*scale);//单个字体的宽
  //int bitmap_w = bitmap_w_one *(chineseNum*2+asciiNum);
  //bitmap_w = alignTo(bitmap_w,8);
  int bitmap_w_true = getBitmapTrueWidth(text,pixels);
  int bitmap_w = alignTo(bitmap_w_true,8);

    *width = bitmap_w;
    *height = bitmap_h;

	if(NULL == inputBitmap) {
	    argbBuf = (unsigned char*)malloc(bitmap_w*bitmap_h*2);
		if(NULL == argbBuf)
		{
			log_e("malloc error");
			free(bitmap);
			pthread_mutex_unlock(&mutex_lock);
			return NULL;
		}

		bitmap = textToBitmap(text,scale,bitmap_w,bitmap_h, NULL);
	    bitmapToArgb1555(argbBuf, bitmap_w*bitmap_h*2, bitmap, bitmap_w,bitmap_h);
		free(bitmap);
	} else {

	    bitmap = textToBitmap(text,scale,bitmap_w,bitmap_h, inputBitmap);
		argbBuf = inputBitmap + bitmap_w * bitmap_h;
		bitmapToArgb1555(argbBuf, bitmap_w*bitmap_h*2, bitmap, bitmap_w,bitmap_h);
	}
	pthread_mutex_unlock(&mutex_lock);
	return (argbBuf);
}

int trueTypeInit(void)
{
	int ret = -1;

    ret = fontinfoInit(font_path);

	return ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

