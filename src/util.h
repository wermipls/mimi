#pragma once

static inline int smin(int a, int b)
{
    return a > b ? b : a;
}

static inline int smax(int a, int b)
{
    return a > b ? a : b;
}
