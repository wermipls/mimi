#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libdragon.h>

#include "range_test.h"
#include "range_live.h"
#include "text.h"
#include "colors.h"
#include "input.h"

struct StickAngles live_compare =
{
    .values = {
      0,  85,
      70, 70,
      85, 0 ,
      70,-70,
      0, -85,
     -70,-70,
     -85, 0 ,
     -70, 70
    }
};

void display_live_ranges() {
    text_set_line_height(11);
    uint32_t c_blue = graphics_make_color(0, 192, 255, 255);
    uint32_t c_green = graphics_make_color(64, 255, 0, 255);
    display_context_t ctx;

    int f = dfs_open("/gfx/point.sprite");
    int size = dfs_size(f);
    sprite_t *point = malloc(size);
    dfs_read(point, size, 1, f);
    dfs_close(f);
    
    int sz_history = 1024;
    struct Vec2 history[sz_history];
    int count = 0;

    for (;;) {
        while ((ctx = display_lock()) == 0) {}
        display_show(ctx);

        graphics_fill_screen(ctx, COLOR_BACKGROUND);
        graphics_set_color(COLOR_FOREGROUND, 0);

        struct controller_data cdata = get_keys_pressed();
        char buf[128];
        controller_scan();
        struct Vec2 v = { cdata.c[0].x, cdata.c[0].y };

        snprintf(buf, sizeof(buf), "x: %d\ny: %d", v.x, v.y);
        text_draw_wordwrap(ctx, 256, 24, 320-64, buf);
        draw_center_cross(ctx);
        graphics_draw_sprite(ctx, v.x + 118, (v.y * -1) + 118, point);
        draw_stick_angles(ctx, live_compare, c_green, 0);

        if (count < sz_history) {
            count += 1;
        }

        history[0] = v;
        for (int i = smin(count, sz_history); i > 0; i--) {
            history[i] = history[i - 1];
            graphics_draw_pixel_trans(ctx, history[i].x + 118, (history[i].y * -1) + 118, c_blue);
        } 

        if (cdata.c[0].start) {
            break;
        }
    }

    free(point);
}
