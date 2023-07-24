#include "OutlineOsd.h"
#include <freetype/ftstroke.h>
#include <iostream>

static FT_Library _ftLib = NULL;
static bool g_fontInit = false;
#ifndef EP_FONT_BOX_PX
#define EP_FONT_BOX_PX 3
#endif
void RasterCallback(const int y, const int count, const FT_Span* const spans, void* const user)
{
    auto p = (CallbackParams*)user;
    p->arg->SpansCallback(y, count, spans, p->t);
}

OutlineOsd::OutlineOsd()
{
    _param = new CallbackParams();
    _param->arg = this;
}

OutlineOsd::~OutlineOsd()
{
    if (_ftFace)
        FT_Done_Face(_ftFace);
    delete _param;

    if (_presetCache)
        delete[] _presetCache;
    if (_cache)
        delete[] _cache;
}

int OutlineOsd::Init(int fontSize, std::string fontPath)
{
    FT_Error err;
    if (!g_fontInit)
    {
        err = FT_Init_FreeType(&_ftLib);
        if (err)
        {
            std::cerr << "init freetype failed:" << err << std::endl;
            return 1;
        }
        g_fontInit = true;
    }

    err = FT_New_Face(_ftLib, fontPath.c_str(), 0, &_ftFace);
    if (err)
    {
        std::cerr << "new face failed:" << err << std::endl;
        return 1;
    }
    FT_Set_Pixel_Sizes(_ftFace, fontSize, 0);
    _fontSize = fontSize;
    return 0;
}

unsigned char* OutlineOsd::Write(int thickness,const char* text, int* bmpWidth, int* bmpHeight)
{
    _step = 0;

    CalcCharcodes(text);

    auto bw = _fontSize * _vCharcodes.size() + (_vCharcodes.size() - 1) * 5;
    auto bh = _fontSize + EP_FONT_BOX_PX;
    if (_presetWidth != (int) bw || _height != (int) bh)
    {
        _presetWidth = bw;
        _height = bh;
        if (_presetCache)
            delete[] _presetCache;
        _presetCache = new unsigned char[_presetWidth * _height * 2];
    }

    ResetBmpColor();

    auto it = _vCharcodes.begin();
    while (it != _vCharcodes.end())
    {
        int ret = WriteChar(*it,thickness);
        if(ret < 0){
            std::cerr << "WriteChar failed it:" << *it << std::endl;
            return NULL;
        }
        ++it;
    }

    AdjustBmp();
    *bmpWidth = _realWidth;
    *bmpHeight = _realHeight;

    return _cache;
}

void OutlineOsd::CalcCharcodes(const char* text)
{
    _vCharcodes.clear();

    auto numChars = strlen(text);
    int ch_in = 0;
    for (size_t i = 0; i < numChars; i++)
    {
        ch_in = text[i];
        unsigned char a = text[i];
        if (0x0E == (a >> 4))
        {
            unsigned char b = text[i + 1];
            unsigned char c = text[i + 2];
            i += 2;
            ch_in = ((a & 0x0F) << 12) | ((b & 0x3F) << 6) | (c & 0x3F);
            _vCharcodes.push_back(ch_in);
        }
        else
        {
            _vCharcodes.push_back(ch_in);
        }
    }
}

int OutlineOsd::WriteChar(int ch,int thickness)
{
    ResetParam();

    FT_Error err;
    err = FT_Load_Glyph(_ftFace, FT_Get_Char_Index(_ftFace, ch), FT_LOAD_NO_BITMAP);
    if (err)
    {
        std::cerr << "ft load glyph failed:" << err << std::endl;
        return -1;
    }
    /*get slot*/
    auto slot = _ftFace->glyph;
    if (slot->format == FT_GLYPH_FORMAT_OUTLINE)
    {
        /*��ȡ����span*/
        _param->t = 0;
        OutlineRender(&slot->outline, _param);

        FT_Stroker stroker;
        err = FT_Stroker_New(_ftLib, &stroker);
        FT_Stroker_Set(stroker,
                       thickness * 64,
                       FT_STROKER_LINECAP_ROUND,
                       FT_STROKER_LINEJOIN_ROUND,
                       2);

        // �ӿ����������span
        FT_Glyph glyph;
        if (FT_Get_Glyph(slot, &glyph) == 0)
        {
            FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
            if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
            {
                // Render the outline spans to the span list
                FT_Outline* o = &(((FT_OutlineGlyph)glyph)->outline);
                _param->t = 1;
                OutlineRender(o, _param);
            }
            // Clean up afterwards.
            FT_Stroker_Done(stroker);
            FT_Done_Glyph(glyph);

            HandleSpansData(_step);

            if (ch>-1 && ch<256 && isdigit(ch))
            {
                _step += (_fontSize >> 1) + 3;
            }
            else if (ch>-1 && ch<256 && isalpha(ch))
                _step += std::max((int)(slot->advance.x >> 6), (_fontSize >> 1) + 3) +1;
            else
                _step += slot->advance.x >> 6;
        }
    }
    return 0;
}

void OutlineOsd::SpansCallback(const int y, const int count, const FT_Span* const spans, unsigned char t)
{
    for (int i = 0; i < count; i++)
    {
        OsdSpan* n = new OsdSpan;
        n->y = y;
        n->x = spans[i].x;
        n->len = spans[i].len;

        _xMin = std::min(n->x, _xMin);
        _xMax = std::max(n->x + spans[i].len - 1, _xMax);
        _yMin = std::min(n->y, _yMin);
        _yMax = std::max(n->y, _yMax);

        if (t == 0)
            _v1.emplace_back(n);
        else
            _v2.emplace_back(n);
    }
}

void OutlineOsd::HandleSpansData(int x)
{
    unsigned short* pixe = (unsigned short*)_presetCache;
    auto it = _v2.begin();
    OsdSpan* n = NULL;
    while (it != _v2.end())
    {
        n = *it;
        for (int i = 0; i < n->len; i++)
        {
            auto m = (_height - 1 - (n->y - _yMin)) * _presetWidth + n->x - _xMin + i + x;
            if (m > _presetWidth * _height || m < 0) {
                std::cout << "font buffer " << m << " too small need adjust it n->len=" << n->len <<std::endl;
                continue;
            }
            pixe[m] = 0x8000;
        }
        delete n;
        ++it;
    }

    it = _v1.begin();
    while (it != _v1.end())
    {
        n = *it;
        for (int i = 0; i < n->len; i++)
        {
            auto m = (_height - 1 - (n->y - _yMin)) * _presetWidth + n->x - _xMin + i + x;
            if (m > _presetWidth * _height || m < 0) {
                std::cout << "font buffer " << m << " too small need adjust n->len=" << n->len << std::endl;
                continue;
            }
            pixe[m] = 0xFFFF;
        }
        delete n;
        ++it;
    }
}

void OutlineOsd::OutlineRender(FT_Outline* const outline, void* usr)
{
    FT_Raster_Params params;
    memset(&params, 0, sizeof(params));
    params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = RasterCallback;
    params.user = usr;
    FT_Outline_Render(_ftLib, outline, &params);
}

void OutlineOsd::ResetParam()
{
    _v1.clear();
    _v2.clear();
    _xMin = 0;
    _xMax = 0;
    _yMin = 0;
    _yMax = 0;
}

void OutlineOsd::ResetBmpColor()
{
    if (!_presetCache)
        return;
    for (size_t i = 0; i < (size_t)(_presetWidth * _height * 2); i += 2)
    {
        _presetCache[i] = 0x00;
        _presetCache[i + 1] = 0x00;
        /*black*/
//		r5g5b5a1[i] = 0xFF;
//		r5g5b5a1[i + 1] = 0xFF;
    }
}

void OutlineOsd::AdjustBmp()
{
    if (_realHeight != _height || _realWidth != _step)
    {
        if (_cache)
            delete[] _cache;
        _realHeight = _height;
        _realWidth = _step;
        _cache = new unsigned char[_realHeight * _realWidth * 2];
    }
    for (int i = 0; i < _height; i++)
    {
        memcpy(_cache + i * _realWidth * 2, _presetCache + i * _presetWidth * 2, _realWidth * 2);
    }
}