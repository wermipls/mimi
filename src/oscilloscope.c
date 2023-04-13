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
        plot_offset = 24,
        count = 0;

    float zoom = 0.4;
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

        controller_scan();
        struct controller_data cdata = get_keys_pressed();

        struct Vec2 v = { cdata.c[0].x, cdata.c[0].y };

        for (int i = count; i > 0; i--) {
            history[i] = history[i - 1];
            draw_aa_line(
                ctx, 
                plot_offset + sz_history - i, 
                70, 
                plot_offset + sz_history - i, 
                70 + history[i].x * zoom, 
                c_blue
            );
            draw_aa_line(
                ctx, 
                plot_offset + sz_history - i, 
                170, 
                plot_offset + sz_history - i, 
                170 + (history[i].y * -1) * zoom, 
                c_green
            );
        }

        history[0] = v;
        draw_aa_line(
            ctx, 
            plot_offset + sz_history, 
            70, 
            plot_offset + sz_history, 
            70 + v.x * zoom, 
            c_blue
        );
        draw_aa_line(
            ctx, 
            plot_offset + sz_history, 
            170, 
            plot_offset + sz_history, 
            170 + (v.y * -1) * zoom, 
            c_green
        );

        if (count < sz_history - 1) {
            count++;
        }

        char buf[128];

        text_set_font(FONT_BOLD);
        snprintf(buf, sizeof(buf), "%3d", v.x);
        text_draw(ctx, 293, 64, buf, ALIGN_RIGHT);

        snprintf(buf, sizeof(buf), "%3d", v.x);
        text_draw(ctx, 293, 164, buf, ALIGN_RIGHT);

        text_set_font(FONT_MEDIUM);
        snprintf(buf, sizeof(buf), "x");
        text_draw(ctx, 300, 64, buf, ALIGN_LEFT);

        snprintf(buf, sizeof(buf), "y");
        text_draw(ctx, 300, 164, buf, ALIGN_LEFT);


        snprintf(buf, sizeof(buf), "Oscilloscope display");
        text_draw(ctx, 160, 15, buf, ALIGN_CENTER);

        text_set_font(FONT_MEDIUM);
        graphics_set_color(graphics_make_color(128, 128, 128, 255), 0);
        text_draw(ctx, 320 - 16, 213, REPO_URL, ALIGN_RIGHT);

        if (cdata.c[0].start) {
            break;
        }
    }
}
