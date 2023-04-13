#pragma once

#include "util.h"

struct StickAngles
{
    union {
        struct {
            struct Vec2 u;
            struct Vec2 ur;
            struct Vec2 r;
            struct Vec2 dr;
            struct Vec2 d;
            struct Vec2 dl;
            struct Vec2 l;
            struct Vec2 ul;
        };
        int values[16];
    };
};

void test_angles(struct StickAngles *a, int testnum);
void display_angles(struct StickAngles a[], int sample_count);
void draw_center_cross(display_context_t ctx, int x_origin);
void draw_stick_angles(display_context_t ctx, struct StickAngles a, uint32_t color, int zoomout, int x);
