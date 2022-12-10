#include <stdlib.h>

#include "input.h"

struct controller_data previous;

void input_read_held(struct controller_data *data)
{
    controller_read(data);
    previous = *data;
}

void input_read_pressed(struct controller_data *data)
{
    struct controller_data current; 
    controller_read(&current);
    *data = current;

    for (int i = 0; i < 4; i++) {
        uint32_t a = ~(~current.c[i].data | previous.c[i].data);
        data->c[i].data = a;
    }

    previous = current;
}
