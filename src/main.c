#include <libdragon.h>

#include "range_test.h"
#include "text.h"

enum Screen
{
    SCR_RANGE_TEST,
    SCR_RANGE_RESULT,
};

int main(void)
{
    display_init(RESOLUTION_320x240, DEPTH_32_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    controller_init();
    dfs_init(DFS_DEFAULT_LOCATION);

    text_init();

    console_set_debug(true);

    enum Screen current_screen = 0;

    struct StickAngles result;

    for (;;) {
        switch (current_screen)
        {
        case SCR_RANGE_TEST:
            test_angles(&result);
            current_screen = SCR_RANGE_RESULT;
            break;
        case SCR_RANGE_RESULT:
            display_angles(&result);
            current_screen = SCR_RANGE_TEST;
        }
    }
}