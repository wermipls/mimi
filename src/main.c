#include <stdio.h>

#include <libdragon.h>

#include "util.h"
#include "drawing.h"

struct Vec2
{
    int x;
    int y;
};

struct StickAngles
{
    struct Vec2 u;
    struct Vec2 ur;
    struct Vec2 r;
    struct Vec2 dr;
    struct Vec2 d;
    struct Vec2 dl;
    struct Vec2 l;
    struct Vec2 ul;
};

struct StickAngles perfect_n64 =
{
    { 0,  85},
    { 70, 70},
    { 85, 0 },
    { 70,-70},
    { 0, -85},
    {-70,-70},
    {-85, 0 },
    {-70, 70},
};

struct StickAngles perfect_hori =
{
    { 0,  100},
    { 75, 75},
    { 100, 0 },
    { 75,-75},
    { 0, -100},
    {-75,-75},
    {-100, 0 },
    {-75, 75},
};

void draw_stick_angles(display_context_t ctx, struct StickAngles a, uint32_t color)
{
    struct Vec2 *v = (struct Vec2*)&a;

    for (int i = 0; i < 8; i++) {
        int j = (i + 1) % 8;
        draw_aa_line(
            ctx, 
            v[i].x + 120, 
            v[i].y + 120, 
            v[j].x + 120,
            v[j].y + 120,
            color);
    }
}

void print_stick_angles(display_context_t ctx, struct StickAngles a)
{
    char buf[1024];
    snprintf(buf, sizeof(buf),
        "%3d up   \n\n"
        "%3d down \n\n"
        "%3d left \n\n" 
        "%3d right\n\n\n\n"
        "%3d ur   \n"
        "%3d      \n\n"
        "%3d ul   \n"
        "%3d      \n\n"
        "%3d dr   \n"
        "%3d      \n\n"
        "%3d dl   \n"
        "%3d      \n\n",
        abs(a.u.y), abs(a.d.y), abs(a.l.x), abs(a.r.x),
        abs(a.ur.x), abs(a.ur.y),
        abs(a.ul.x), abs(a.ul.y),
        abs(a.dr.x), abs(a.dr.y),
        abs(a.dl.x), abs(a.dl.y));

    graphics_draw_text(ctx, 240, 16, buf);
}

void test_angles(struct StickAngles *a)
{
    static const char *angles[] =
    {
        "up",
        "up-right",
        "right",
        "down-right",
        "down",
        "down-left",
        "left",
        "up-left",
    };

    struct Vec2 *v = (struct Vec2*)a;

    for (int i = 0; i < 8; i++) {
        char buf[128];
        snprintf(buf, sizeof(buf), "hold %s and press a", angles[i]);
        int a_held = 1;

        for (;;) {
            display_context_t ctx;
            while ((ctx = display_lock()) == 0) {}
            graphics_fill_screen(ctx, graphics_make_color(0,0,0,255));
            graphics_draw_text(ctx, 32, 128, buf);
            display_show(ctx);

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
    }
}

int main(void)
{
    display_init(RESOLUTION_320x240, DEPTH_32_BPP, 2, GAMMA_CORRECT_DITHER, ANTIALIAS_RESAMPLE);
    controller_init();

    console_set_debug(true);

    struct StickAngles a;

    test_angles(&a);

    uint32_t c  = graphics_make_color(0, 64, 255, 255);
    uint32_t c2 = graphics_make_color(64, 255, 0, 255);

    for (;;) {
        display_context_t ctx;
        while ((ctx = display_lock()) == 0) {}

        graphics_fill_screen(ctx, graphics_make_color(0,0,0,255));

        draw_stick_angles(ctx, perfect_n64, c2);
        draw_stick_angles(ctx, a, c);
        print_stick_angles(ctx, a);
        display_show(ctx);
    }
}