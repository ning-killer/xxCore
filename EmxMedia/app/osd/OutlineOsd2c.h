#ifndef _OUTLINEOSD2C_H_
#define _OUTLINEOSD2C_H_
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*初始化osd字体*/
int outline_osd_init(int fontSize,char *fontPath);
/*释放osd资源*/
void outline_osd_uninit(int handle);
/*传入字符信息，放回rgba图片数据
handle： 句柄
text:传入的字符
bmpWidth,bmpHeight: 回传实际图片的宽高
return 位图，不需要释放
*/
unsigned char* outline_osd_create_bmp(int handle, const char* text,int* bmpWidth, int* bmpHeight,int thickness);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif