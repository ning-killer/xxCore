#pragma once
#include <string>
#include <vector>
#include <ft2build.h>
#include <freetype/freetype.h>

class CallbackParams;
class OsdSpan;

class OutlineOsd
{
public:
    OutlineOsd();
    ~OutlineOsd();

    int Init(int fontSize,std::string fontPath);

    unsigned char* Write(int thickness,const char* text, int* bmpWidth, int* bmpHeight);

    void SpansCallback(const int y, const int count, const FT_Span* const spans, unsigned char t);
private:
    void CalcCharcodes(const char* text);

    int WriteChar(int ch,int thickness);

    void OutlineRender(FT_Outline* const outline, void* usr);

    void ResetParam();

    void ResetBmpColor();

    void HandleSpansData(int x);

    void AdjustBmp();
private:
    FT_Face _ftFace = NULL;
    int _fontSize = 33;
    std::vector<OsdSpan*> _v1;
    std::vector<OsdSpan*> _v2;
    CallbackParams* _param = NULL;

    int _xMin = 0;
    int _xMax = 0;
    int _yMin = 0;
    int _yMax = 0;

    int _step = 0;

    std::vector<int> _vCharcodes;

    int _presetWidth = 0;
    int _height = 0;
    int _realWidth = 0;
    int _realHeight = 0;
    unsigned char* _presetCache = NULL;/*x1r5g5b5*/
    unsigned char* _cache = NULL;
};

class OsdSpan
{
public:
    int y = 0;
    int x = 0;
    int len = 0;
    unsigned char coverage = 0xFF;
};

class CallbackParams
{
public:
    OutlineOsd* arg;
    unsigned char t;
};