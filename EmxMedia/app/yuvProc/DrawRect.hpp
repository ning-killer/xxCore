//
// Created by SimonXiao on 2022/3/27.
//

#ifndef EMX_DrawRect_HPP
#define EMX_DrawRect_HPP

#include <cstdint>
#include "media/MediaRegion.hpp"
namespace Emx {

    class DrawRect {
    public:
        struct Color {
            uint8_t y;
            uint8_t c0;
            uint8_t c1;
        };

        enum class ColorE : uint8_t {
            Green = 0,
            Red = 1,
            Blue = 2,
            White = 3,
            Black = 4,
            Max,
        };

        static void Nv21(ColorE color, uint8_t *y, uint8_t *uv, Rect &rect, int stride, int lineWidth) {
            Nvxx(&nv21[(int) color], y, uv, rect, stride, lineWidth);
        }

        static void Nv12(ColorE color, uint8_t *y, uint8_t *uv, Rect &rect, int stride, int lineWidth) {
            Nvxx(&nv12[(int) color], y, uv, rect, stride, lineWidth);
        }

    private:
        static Color nv21[(int) ColorE::Max];
        static Color nv12[(int) ColorE::Max];

        static void Nvxx(DrawRect::Color *color, uint8_t *y, uint8_t *uv, Rect &rect, int stride, int lineWidth);
    };

}

#endif //EMX_DrawRect_HPP
