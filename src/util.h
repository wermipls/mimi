#pragma once

static inline int smin(int a, int b)
{
    return a > b ? b : a;
}

static inline int smax(int a, int b)
{
    return a > b ? a : b;
}

static inline float get_angle(float x, float y)
{
    return atan2f(x, y) * 180 / 3.14159;
}
