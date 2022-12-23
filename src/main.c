#include <libdragon.h>

#include "range_test.h"
#include "text.h"
#include "input.h"
#include "colors.h"

enum Screen
{
    SCR_MAIN_MENU,
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

    struct StickAngles result[9];
    display_context_t ctx;
    int sample_count;

    for (;;) {
        switch (current_screen)
        {
        case SCR_MAIN_MENU:
            int menu_selection = 0;

            for (;;) {
                while ((ctx = display_lock()) == 0) {}

                graphics_fill_screen(ctx, COLOR_BACKGROUND);

                graphics_set_color(COLOR_FOREGROUND, 0);
                text_set_font(FONT_BOLD);
                text_draw(ctx, 32, 32, "mimi " ROM_VERSION, ALIGN_LEFT);

                static const char *options[] = {
                    "Range test (1 sample)",
                    "Range test (3 samples)",
                    "Range test (5 samples)",
                    "About",
                    "Quit",
                };

                int menu_options = sizeof(options)/sizeof(char*);
                text_set_font(FONT_MEDIUM);

                for (int i = 0; i < menu_options; i++) {
                    int x = 42;
                    if (i == menu_selection) {
                        text_draw(ctx, x - 10, 52 + i*10, ">", ALIGN_LEFT);
                    }

                    text_draw(ctx, x, 52 + i*10, options[i], ALIGN_LEFT);
                }

                display_show(ctx);

                struct controller_data cdata;
                input_read_pressed(&cdata);

                if (cdata.c[0].A) {
                    switch (menu_selection)
                    {
                    case 0:
                        sample_count = 1;
                        current_screen = SCR_RANGE_TEST;
                        break;
                    case 1:
                        sample_count = 3;
                        current_screen = SCR_RANGE_TEST;
                        break;
                    case 2:
                        sample_count = 5;
                        current_screen = SCR_RANGE_TEST;
                        break;
                    }
                }

                if (current_screen != SCR_MAIN_MENU) {
                    break;
                }

                if (cdata.c[0].up) {
                    menu_selection--;
                    if (menu_selection < 0) menu_selection = menu_options - 1;
                } else if (cdata.c[0].down) {
                    menu_selection++;
                    if (menu_selection >= menu_options) menu_selection = 0;
                }
            }
            break;
        case SCR_RANGE_TEST:
            for (int i = 0; i < sample_count; i++) {
                test_angles(&result[i]);
            }
            current_screen = SCR_RANGE_RESULT;
            break;
        case SCR_RANGE_RESULT:
            display_angles(result, sample_count);
            current_screen = SCR_MAIN_MENU;
        }
    }
}