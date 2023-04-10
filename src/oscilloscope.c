#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libdragon.h>

#include "range_test.h"
#include "oscilloscope.h"
#include "drawing.h"
#include "text.h"
#include "colors.h"
#include "input.h"

void display_oscilloscope() {
    int line_height = 11,
        sz_history = 240,
        count = 0;

    float zoom = 0.5;
    uint32_t c_blue = graphics_make_color(0, 192, 255, 255);
    uint32_t c_green = graphics_make_color(64, 255, 0, 255);
    struct Vec2 history[sz_history];

    text_set_line_height(line_height);
    display_context_t ctx;

    for (;;) {
        while ((ctx = display_lock()) == 0) {}
        display_show(ctx);

        graphics_fill_screen(ctx, COLOR_BACKGROUND);
        graphics_set_color(COLOR_FOREGROUND, 0);

        struct controller_data cdata = get_keys_pressed();
        controller_scan();

        struct Vec2 v = { cdata.c[0].x, cdata.c[0].y };
        if (count < sz_history - 1) {
            count++;
        }

        for (int i = count; i > 0; i--) {
            history[i] = history[i - 1];
            draw_aa_line(ctx, sz_history - i, 80, sz_history - i, 80 + history[i].x * zoom, c_blue);
            draw_aa_line(ctx, sz_history - i, 180, sz_history - i, 180 + history[i].y * zoom, c_green);
        }

        history[0] = v;

        char buf[128], lbl_buf[128];
        snprintf(buf, sizeof(buf), "x: %d", v.x);
        text_draw_wordwrap(ctx, 256, 74, 320-64, buf);

        snprintf(buf, sizeof(buf), "y: %d", v.y);
        text_draw_wordwrap(ctx, 256, 174, 320-64, buf);

        snprintf(lbl_buf, sizeof(lbl_buf), "Oscilloscope display");
        text_draw(ctx, 20, 15, lbl_buf, ALIGN_LEFT);

        text_set_font(FONT_MEDIUM);
        graphics_set_color(graphics_make_color(128, 128, 128, 255), 0);
        text_draw(ctx, 320 - 16, 213, REPO_URL, ALIGN_RIGHT);

        if (cdata.c[0].start) {
            break;
        }
    }
}
