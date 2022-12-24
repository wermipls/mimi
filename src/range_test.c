#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libdragon.h>

#include "range_test.h"
#include "colors.h"
#include "drawing.h"
#include "text.h"
#include "util.h"

struct StickAngles perfect_n64 =
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

struct StickAngles perfect_hori =
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

enum Comparison
{
    COMP_NONE,
    COMP_N64,
    COMP_HORI,

    COMP_MAX,
};

struct StickAngles *comparisons[] =
{
    NULL,
    &perfect_n64,
    &perfect_hori,
};

const char *comparison_names[] =
{
    "Benchmark results",
    "Perfect N64 OEM comparison",
    "Perfect Horipad Mini comparison",
};

void draw_stick_angles(display_context_t ctx, struct StickAngles a, uint32_t color)
{
    struct Vec2 *v = (struct Vec2*)&a;

    for (int i = 0; i < 8; i++) {
        int j = (i + 1) % 8;
        draw_aa_line(
            ctx, 
            120 + v[i].x, 
            120 - v[i].y, 
            120 + v[j].x,
            120 - v[j].y,
            color);
    }
}

void draw_center_cross(display_context_t ctx)
{
    int x, y;
    y = 120;
    for (x = 0; x < 240; x++) {
        int i = smin(240 - abs(240 - x * 2), 120);
        graphics_draw_pixel_trans(ctx, x, y, graphics_make_color(255, 255, 255, i));
    }

    x = 120;
    for (y = 0; y < 240; y++) {
        int i = smin(240 - abs(240 - y * 2), 120);
        graphics_draw_pixel_trans(ctx, x, y, graphics_make_color(255, 255, 255, i));
    }
}

uint32_t get_range_color_cardinal(int a)
{
    if (a >= 80) {
        return graphics_make_color(0, 255, 64, 255);
    } else if (a >= 75) {
        return graphics_make_color(192, 255, 0, 255);
    } else if (a >= 70) {
        return graphics_make_color(255, 128, 64, 255);
    } else {
        return graphics_make_color(255, 64, 0, 255);
    }
}

uint32_t get_range_color_diagonal(int x, int y)
{
    float euclidean = sqrtf(x*x + y*y);

    return get_range_color_cardinal(euclidean / 1.125);
}

uint32_t get_angle_color(float angle)
{

    float diff = fabsf(45.0f - angle);

    if (diff < 1) {
        return graphics_make_color(0, 255, 64, 255);
    } else if (diff < 3) {
        return graphics_make_color(192, 255, 0, 255);
    } else if (diff < 5) {
        return graphics_make_color(255, 128, 64, 255);
    } else {
        return graphics_make_color(255, 64, 0, 255);
    }
}

void print_stick_angles(display_context_t ctx, struct StickAngles a)
{
    char buf[1024];
    snprintf(buf, sizeof(buf),
        "up   \n"
        "down \n"
        "left \n" 
        "right\n\n"
        "UR\n\n\n"
        "UL\n\n\n"
        "DR\n\n\n"
        "DL"
    );

    int y = 15;

    graphics_set_color(COLOR_FOREGROUND, 0);

    text_set_font(FONT_MEDIUM);
    text_draw(ctx, 270, y, buf, ALIGN_LEFT);

    text_set_font(FONT_BOLD);
    int cardinals[] = {a.u.y, -a.d.y, -a.l.x, a.r.x};

    for (int i = 0; i < 4; i++) {
        snprintf(buf, sizeof(buf), "%3d", cardinals[i]);
        uint32_t c = get_range_color_cardinal(cardinals[i]);
        graphics_set_color(c, 0);
        text_draw(ctx, 263, y, buf, ALIGN_RIGHT);
        y += 10;
    }

    int diagonals[] = {
         a.ur.x,  a.ur.y,
        -a.ul.x,  a.ul.y,
         a.dr.x, -a.dr.y,
        -a.dl.x, -a.dl.y,
    };

    y += 10;

    for (int i = 0; i < 8; i += 2) {
        snprintf(buf, sizeof(buf), "%3d\n%3d", diagonals[i], diagonals[i+1]);
        uint32_t c = get_range_color_diagonal(smax(0, diagonals[i]), smax(0, diagonals[i+1]));
        graphics_set_color(c, 0);
        text_draw(ctx, 263, y, buf, ALIGN_RIGHT);
        y += 30;
    }

    float angles[] = {
        get_angle(diagonals[0], diagonals[1]),
        get_angle(diagonals[2], diagonals[3]),
        get_angle(diagonals[4], diagonals[5]),
        get_angle(diagonals[6], diagonals[7]),
    };

    y = 15 + 60;
    text_set_font(FONT_MEDIUM);

    for (int i = 0; i < 4; i++) {
        snprintf(buf, sizeof(buf), "%2.1f" SYMBOL_DEGREES, angles[i]);
        uint32_t c = get_angle_color(angles[i]);
        graphics_set_color(c, 0);
        text_draw(ctx, 270, y, buf, ALIGN_LEFT);
        y += 30;
    }
}

void test_angles(struct StickAngles *a)
{
    static const char *angles[] =
    {
        "Up",
        "Up-Right",
        "Right",
        "Down-Right",
        "Down",
        "Down-Left",
        "Left",
        "Up-Left",
    };

    static const char *gfx[] =
    {
        "/gfx/stick_0.sprite", 
        "/gfx/stick_1.sprite",
        "/gfx/stick_2.sprite",
        "/gfx/stick_3.sprite",
        "/gfx/stick_4.sprite",
        "/gfx/stick_5.sprite",
        "/gfx/stick_6.sprite",
        "/gfx/stick_7.sprite",
    };

    struct Vec2 *v = (struct Vec2*)a;

    graphics_set_color(COLOR_FOREGROUND, 0);
    text_set_font(FONT_BOLD);

    for (int i = 0; i < 8; i++) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Hold %s and press A", angles[i]);
        int a_held = 1;

        int f = dfs_open(gfx[i]);
        int size = dfs_size(f);
        sprite_t *stick = malloc(size);
        dfs_read(stick, size, 1, f);
        dfs_close(f);

        display_context_t ctx;
        while ((ctx = display_lock()) == 0) {}
        graphics_fill_screen(ctx, COLOR_BACKGROUND);
        graphics_draw_sprite(ctx, (320-128)/2, (240-128)/2, stick);
        text_draw(ctx, 320/2, 32, buf, ALIGN_CENTER);
        display_show(ctx);

        for (;;) {
            struct controller_data data;
            controller_read(&data);
            if (data.c[0].A && !a_held) {
                v[i].x = data.c[0].x;
                v[i].y = data.c[0].y;
                a_held = 1;
                break;
            } else if (!data.c[0].A) {
                a_held = 0;
            }
        }

        free(stick);
    }
}

struct StickAngles find_median(struct StickAngles a[], int n)
{
    struct StickAngles median;

    for (int i = 0; i < 16; i++) {
        int sorted[n];

        for (int j = 0; j < n; j++) {
            sorted[j] = a[j].values[i];
        }

        for (int j = 0; j < n; j++) {
            int lowest_idx = j;

            for (int k = j; k < n; k++) {
                if (sorted[k] < sorted[lowest_idx]) {
                    lowest_idx = k;
                }
            }

            int tmp = sorted[j];
            sorted[j] = sorted[lowest_idx];
            sorted[lowest_idx] = tmp;
        }

        if (n % 2) {
            median.values[i] = sorted[(n-1)/2];
        } else {
            median.values[i] = (sorted[n/2-1] + sorted[n/2]) / 2;
        }
    }

    return median;
}

float find_standard_deviation(struct StickAngles a[], int n)
{
    if (n < 2) return -1;

    float values[16*n];

    for (int i = 0; i < 16; i++) {
        float mean = 0;
        float *p = &values[n*i];

        for (int j = 0; j < n; j++) {
            p[j] = a[j].values[i];
            mean += p[j];
        }

        mean = mean / (float)n;

        // the values need to be normalized so each notch's axis
        // becomes comparable to each other
        for (int j = 0; j < n; j++) {
            p[j] -= mean;
        }
    }

    float variance = 0;

    for (int i = 0; i < 16 * n; i++) {
        float v = values[i];
        variance += v * v;
    }
    variance = variance / (float)(16*n - 1);

    return sqrtf(variance);
}

void display_angles(struct StickAngles a[], int sample_count)
{
    enum Comparison current_comparison = COMP_NONE;
    display_context_t ctx;

    uint32_t c  = graphics_make_color(0, 192, 255, 255);
    uint32_t c2 = graphics_make_color(64, 255, 0, 255);
    uint32_t c3 = graphics_make_color(64, 64, 64, 255);

    struct StickAngles median = find_median(a, sample_count);

    for (;;) {
        while ((ctx = display_lock()) == 0) {}

        graphics_fill_screen(ctx, COLOR_BACKGROUND);

        draw_center_cross(ctx);
        if (comparisons[current_comparison]) {
            draw_stick_angles(ctx, *comparisons[current_comparison], c2);
        }
        for (int i = 0; i < sample_count; i++) {
            draw_stick_angles(ctx, a[i], c3);
        }
        draw_stick_angles(ctx, median, c);
        print_stick_angles(ctx, median);
        
        graphics_set_color(COLOR_FOREGROUND, 0);
        int y = 15 + 10*17;

        char buf[32];
        snprintf(buf, sizeof(buf), "%d", sample_count);
        text_set_font(FONT_BOLD);
        text_draw(ctx, 263, y, buf, ALIGN_RIGHT);

        text_set_font(FONT_MEDIUM);
        if (sample_count == 1) {
            text_draw(ctx, 270, y, "test", ALIGN_LEFT);
        } else {
            text_draw(ctx, 270, y, "tests", ALIGN_LEFT);
        }
        
        y += 10;
        if (sample_count > 1) {
            float sd = find_standard_deviation(a, sample_count);
            snprintf(buf, sizeof(buf), "%.2f", sd);

            text_set_font(FONT_BOLD);
            text_draw(ctx, 263, y, buf, ALIGN_RIGHT);

            text_set_font(FONT_MEDIUM);
            text_draw(ctx, 270, y, "std dev", ALIGN_LEFT);
        }

        text_set_font(FONT_BOLD);
        graphics_set_color(COLOR_FOREGROUND, 0);
        text_draw(ctx, 120, 15, comparison_names[current_comparison], ALIGN_CENTER);

        text_set_font(FONT_MEDIUM);
        graphics_set_color(graphics_make_color(128, 128, 128, 255), 0);
        text_draw(ctx, 320 - 16, 213, REPO_URL, ALIGN_RIGHT);

        display_show(ctx);

        controller_scan();
        struct controller_data cdata = get_keys_down();
        if (cdata.c[0].start) {
            return;
        }

        if (cdata.c[0].L) {
            if (current_comparison == 0) {
                current_comparison = COMP_MAX - 1;
            } else {
                current_comparison--;
            }
        } else if (cdata.c[0].R) {
            current_comparison++;
            if (current_comparison >= COMP_MAX) {
                current_comparison = 0;
            }
        }
    }
    
}
