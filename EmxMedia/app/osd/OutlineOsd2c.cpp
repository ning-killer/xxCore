#include "OutlineOsd.h"
#include "OutlineOsd2c.h"

#define OSD_HANDLE_MAX 8
OutlineOsd * osd_handle[OSD_HANDLE_MAX];
static int osd_handle_flag[OSD_HANDLE_MAX];
int outline_osd_init(int fontSize,char *fontPath)
{
    int i =0;
    for(i = 0;i< OSD_HANDLE_MAX;i++) {
        if(osd_handle_flag[i] == 0) {
            osd_handle[i] = new OutlineOsd;
            if(osd_handle[i]->Init(fontSize,fontPath) == 0) {
                osd_handle_flag[i] = 1;
                return i;
            }else{
                return -1;
            }
        }
    }
    return -1;
}

void outline_osd_uninit(int handle)
{
    if(osd_handle[handle]){
        delete osd_handle[handle];
        osd_handle_flag[handle] = 0;
    }
}

unsigned char* outline_osd_create_bmp(int handle, const char* text,int* bmpWidth, int* bmpHeight,int thickness)
{
    if(text == NULL){
        return NULL;
    }
    if(handle < 0 || handle >= OSD_HANDLE_MAX){
        return NULL;
    }
    if(osd_handle[handle]){
        return osd_handle[handle]->Write(thickness,text,bmpWidth,bmpHeight);
    }
    return NULL;
}
