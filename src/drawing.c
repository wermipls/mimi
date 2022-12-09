#include <stdlib.h>

#include "drawing.h"

static inline uint32_t color_alpha(uint32_t c, uint8_t i)
{
    return (c & 0xFFFFFF00) | i;
}

__attribute__ ((optimize(0))) void draw_aa_line (display_context_t ctx, int x0, int y0, int x1, int y1, uint32_t c)
{
    if (x0 == x1 || y0 == y1) {
        graphics_draw_line(ctx, x0, y0, x1, y1, c);
        return;
    }

    if (x0 > x1) {
        int tmp = x0;
        x0 = x1;
        x1 = tmp;
        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    int h = x1 - x0;
    int v = y1 - y0;

    bool rising = v < 0;
    v = abs(v);
    bool steep  = h < v;
    int inc = rising ? -1 : 1;

    if (steep) {
        uint16_t f = ((uint32_t)h << 16) / v;
        int x = x0;
        uint16_t a = 0;

        for (int y = y0; y != y1; y += inc) {
            uint8_t i = (0xFFFF - a) >> 8;
            graphics_draw_pixel_trans(ctx, x, y, color_alpha(c, i));
            i = a >> 8;
            graphics_draw_pixel_trans(ctx, x+1, y, color_alpha(c, i));
            uint16_t b = a + f;
            if (b <= a) {
                x++;
            }
            a = b;
        }
    } else {
        uint16_t f = ((uint32_t)v << 16) / h;
        int y = y0;
        uint16_t a = 0;

        for (int x = x0; x < x1; x++) {
            uint8_t i = (0xFFFF - a) >> 8;
            graphics_draw_pixel_trans(ctx, x, y, color_alpha(c, i));
            i = a >> 8;
            graphics_draw_pixel_trans(ctx, x, y+inc, color_alpha(c, i));
            uint16_t b = a + f;
            if (b <= a) {
                y += inc;
            }
            a = b;
        }
    }

    graphics_draw_pixel(ctx, x1, y1, c);
}
