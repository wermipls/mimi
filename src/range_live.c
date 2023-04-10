#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libdragon.h>

#include "range_test.h"
#include "range_live.h"
#include "text.h"
#include "colors.h"
#include "input.h"

struct StickAngles live_compare_oem =
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

struct StickAngles live_compare_hori =
{
    .values = {
      0,  100,
      75, 75,
      100,0,
      75,-75,
      0, -100,
     -75,-75,
     -100,0,
     -75, 75
    }
};

struct StickAngles *live_comparisons[] = {
    NULL,
    &live_compare_oem,
    &live_compare_hori,
};

uint32_t get_comparison_color(int comparison) {
    switch (comparison) {
        case 0:
            return graphics_make_color(0, 192, 255, 255);
        case 1:
            return graphics_make_color(64, 255, 0, 255);
        case 2:
            return graphics_make_color(255, 0, 192, 255);

    }
    return graphics_make_color(64, 64, 64, 255);
}

void display_live_ranges() {
    int count = 0, 
        line_height = 11,
        show_history = 1, 
        sz_history = 1024,
        current_comparison = 1,
        comparison_count = sizeof(live_comparisons) / sizeof(0);
    text_set_line_height(line_height);
    display_context_t ctx;

    int f = dfs_open("/gfx/point.sprite");
    int size = dfs_size(f);
    sprite_t *point = malloc(size);
    dfs_read(point, size, 1, f);
    dfs_close(f);

    struct Vec2 history[sz_history];

    for (;;) {
        while ((ctx = display_lock()) == 0) {}
        display_show(ctx);

        graphics_fill_screen(ctx, COLOR_BACKGROUND);
        graphics_set_color(COLOR_FOREGROUND, 0);

        struct controller_data cdata = get_keys_pressed();
        char buf[128];

        controller_scan();
        struct Vec2 v = { cdata.c[0].x, cdata.c[0].y };

        snprintf(buf, sizeof(buf), "x\ny");
        text_set_font(FONT_MEDIUM);
        text_draw(ctx, 270, 15, buf, ALIGN_LEFT);

        text_set_font(FONT_BOLD);
        snprintf(buf, sizeof(buf), "%3d\n%3d", v.x, v.y);
        text_draw(ctx, 263, 15, buf, ALIGN_RIGHT);

        draw_center_cross(ctx);
        if (current_comparison > 0) {
            draw_stick_angles(
                ctx, 
                *live_comparisons[current_comparison], 
                get_comparison_color(current_comparison), 
                0
            );
        }

        snprintf(buf, sizeof(buf), "Live range display");
        text_draw(ctx, 120, 15, buf, ALIGN_CENTER);

        if (show_history == 1) {
            int history_update = 0;
            if (v.x != history[0].x || v.y != history[0].y) {
                history_update = 1;
                if (count < sz_history - 1) {
                    count++;
                }

                history[0] = v;
            }

            for (int i = count; i > 0; i--) {
                if (history_update == 1) history[i] = history[i - 1];
                uint32_t color = get_comparison_color(0);
                graphics_draw_pixel_trans(ctx, history[i].x + 120, (history[i].y * -1) + 120, color);
            } 
        }

        graphics_draw_sprite(ctx, v.x + 118, (v.y * -1) + 118, point);

        if (cdata.c[0].start) {
            break;
        }

        cdata = get_keys_down_filtered();
        if (cdata.c[0].Z) {
            show_history = abs(show_history - 1);
            if (show_history == 0) count = 0;
        }

        text_set_font(FONT_MEDIUM);
        graphics_set_color(graphics_make_color(128, 128, 128, 255), 0);
        text_draw(ctx, 320 - 16, 213, REPO_URL, ALIGN_RIGHT);

        if (cdata.c[0].left || cdata.c[0].L) {
            current_comparison--;
            if (current_comparison < 0) current_comparison += comparison_count;
        }

        if (cdata.c[0].right || cdata.c[0].R) {
            current_comparison = (current_comparison + 1) % comparison_count;
        }
    }

    free(point);

}
