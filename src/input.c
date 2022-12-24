#include "input.h"

struct controller_data get_keys_down_filtered()
{
    struct controller_data d = get_keys_down();
    for (int i = 0; i < 4; i++) {
        if (d.c[0].err != ERROR_NONE) {
            d.c[0].data = 0;
        }
    }
    return d;
}
