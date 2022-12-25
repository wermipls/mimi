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