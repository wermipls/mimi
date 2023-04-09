#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libdragon.h>

#include "range_test.h"
#include "range_live.h"
#include "text.h"
#include "colors.h"
#include "input.h"

void display_live_ranges() {
    text_set_line_height(11);
    uint32_t c_white = graphics_make_color(255, 255, 255, 255);
    display_context_t ctx;
    for (;;) {
        while ((ctx = display_lock()) == 0) {}
        display_show(ctx);

        graphics_fill_screen(ctx, COLOR_BACKGROUND);
        graphics_set_color(COLOR_FOREGROUND, 0);

        struct controller_data cdata = get_keys_pressed();
        char buf[128];
        controller_scan();
        struct Vec2 v = { cdata.c[0].x, cdata.c[0].y };

        snprintf(buf, sizeof(buf), "%d %d", v.x, v.y);
        text_draw_wordwrap(ctx, 256, 24, 320-64, buf);
        draw_center_cross(ctx);
        graphics_draw_pixel(ctx, v.x + 120, (v.y * -1) + 120, c_white);

    }
}
