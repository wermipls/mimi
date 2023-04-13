#include <libdragon.h>
#include <exception.h>
#include <stdlib.h>

#include "range_test.h"
#include "range_live.h"
#include "oscilloscope.h"
#include "text.h"
#include "colors.h"
#include "input.h"

enum Screen
{
    SCR_MAIN_MENU,
    SCR_HELP,
    SCR_ABOUT,
    SCR_RANGE_TEST,
    SCR_RANGE_RESULT,
    SCR_LIVE,
    SCR_OSCOPE,
};

void reset_handler(exception_t *ex)
{
    if (ex->type != EXCEPTION_TYPE_RESET) {
        exception_default_handler(ex);
        return;
    }

    abort();
}

int main(void)
{
    display_init(RESOLUTION_320x240, DEPTH_32_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    controller_init();
    dfs_init(DFS_DEFAULT_LOCATION);

    text_init();

    console_set_debug(true);
    register_exception_handler(reset_handler);

    enum Screen current_screen = 0;

    struct StickAngles result[9];
    display_context_t ctx;
    int sample_count = -1;
    int is_unsaved_result = 0;

    for (;;) {
        switch (current_screen)
        {
        case SCR_MAIN_MENU:
            static int menu_selection = 0;

            text_set_line_height(11);
            for (;;) {
                while ((ctx = display_lock()) == 0) {}

                graphics_fill_screen(ctx, COLOR_BACKGROUND);

                graphics_set_color(COLOR_FOREGROUND, 0);
                text_set_font(FONT_BOLD);
                text_draw(ctx, 32, 24, "mimi git-" ROM_VERSION " (built on " __DATE__ ")", ALIGN_LEFT);

                static const char *options[] = {
                    "Range test (1 sample)",
                    "Range test (3 samples)",
                    "Range test (5 samples)",
                    "Display last range result",
                    "Live range display",
                    "Oscilloscope display",
                    "Help",
                    "About",
                };

                int menu_options = sizeof(options)/sizeof(char*);
                text_set_font(FONT_MEDIUM);

                for (int i = 0; i < menu_options; i++) {
                    int x = 42;
                    if (i == menu_selection) {
                        text_draw(ctx, x - 10, 44 + i*11, ">", ALIGN_LEFT);
                    }

                    text_draw(ctx, x, 44 + i*11, options[i], ALIGN_LEFT);
                }

                display_show(ctx);

                controller_scan();
                struct controller_data cdata = get_keys_down_filtered();

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
                    case 3:
                        if (sample_count > 0) {
                            current_screen = SCR_RANGE_RESULT;
                        }
                        break;
                    case 4:
                        current_screen = SCR_LIVE;
                        break;
                    case 5:
                        current_screen = SCR_OSCOPE;
                        break;
                    case 6:
                        current_screen = SCR_HELP;
                        break;
                    case 7:
                        current_screen = SCR_ABOUT;
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
        case SCR_ABOUT:
            text_set_line_height(11);
            for (;;) {
                while ((ctx = display_lock()) == 0) {}

                graphics_fill_screen(ctx, COLOR_BACKGROUND);

                graphics_set_color(COLOR_FOREGROUND, 0);
                text_set_font(FONT_BOLD);

                text_draw(ctx, 32, 24, "About", ALIGN_LEFT);

                text_set_font(FONT_MEDIUM);

                text_draw_wordwrap(ctx, 32, 44, 320-64, 
                    "mimi controller test ROM by wermi\n"
                    "version " ROM_VERSION ", built on " __DATE__ "\n\n"
                    REPO_URL "\n\n"
                    "Enter Command font by Font End Dev (fontenddev.com), "
                    "licensed under CC BY 4.0\n\n"
                    "This ROM is heavily inspired by sanni's controllertest "
                    "port for N64, as well as max257612's fork of it, however "
                    "it is written completely from scratch.\n\n"
                );

                display_show(ctx);

                controller_scan();
                struct controller_data cdata = get_keys_down_filtered();
                
                if (cdata.c[0].A || cdata.c[0].B || cdata.c[0].start) {
                    current_screen = SCR_MAIN_MENU;
                    break;
                }
            }
            break;
        case SCR_HELP:
            const char *page_names[] = {
                "Basic controls",
                "Basic controls cont.",
                "Range testing",
                "Range testing cont.",
                "Range testing cont.",
                "Live range display",
                "Oscilloscope display",
            };
            const int pages = sizeof(page_names) / sizeof(char*);
            int page = 0;

            text_set_line_height(11);
            for (;;) {
                while ((ctx = display_lock()) == 0) {}

                graphics_fill_screen(ctx, COLOR_BACKGROUND);
                graphics_set_color(COLOR_FOREGROUND, 0);

                text_set_font(FONT_BOLD);
                text_draw(ctx, 32, 24, page_names[page], ALIGN_LEFT);

                if (page < pages - 1) {
                    text_draw(ctx, 320-32, 205, "Next page >>", ALIGN_RIGHT);
                }

                if (page > 0) {
                    text_draw(ctx, 32, 205, "<< Prev. page", ALIGN_LEFT);
                }

                text_set_font(FONT_MEDIUM);

                switch (page) 
                {
                case 0:
                    text_set_font(FONT_MEDIUM);
                    text_draw(ctx, 32, 44 + 11*0,
                        "* D-Pad Left/Right or L/R - change page\n"
                        "* A/B - return to main menu", ALIGN_LEFT);
                    text_set_font(FONT_BOLD);
                    text_draw(ctx, 32, 44 + 11*3,
                        "In the main menu:", ALIGN_LEFT);
                    text_set_font(FONT_MEDIUM);
                    text_draw(ctx, 32, 44 + 11*4,
                        "* D-Pad - select option\n"
                        "* A - confirm selection\n", ALIGN_LEFT);
                    text_set_font(FONT_BOLD);
                    text_draw(ctx, 32, 44 + 11*7,
                        "On the range test result screen:", ALIGN_LEFT);
                    text_set_font(FONT_MEDIUM);
                    text_draw(ctx, 32, 44 + 11*8,
                        "* L/R - switch between range comparisons\n"
                        "* D-Pad Up/Down - switch between measurements\n"
                        "* D-Pad Left/Right - switch between\n"
                        "  example ranges and result measurements\n"
                        "* Z - change zoom\n"
                        "* Start - return to main menu\n", ALIGN_LEFT);
                    break;
                case 1:
                    text_set_font(FONT_BOLD);
                    text_draw_wordwrap(ctx, 32, 44 + (11 * 0), 320-64, 
                        "On the live range testing screen:\n");
                    text_set_font(FONT_MEDIUM);
                    text_draw_wordwrap(ctx, 32, 44 + (11 * 1), 320-64,
                        "* A - toggle history display\n"
                        "* B - clear history display\n"
                        "* Z - change zoom\n"
                        "* L/R, D-Pad Left/Right - cycle example ranges\n"
                        "* Start - return to main menu\n");
                    text_set_font(FONT_BOLD);
                    text_draw_wordwrap(ctx, 32, 44 + (11 * 7), 320-64, 
                        "On the oscilloscope screen:\n");
                    text_set_font(FONT_MEDIUM);
                    text_draw_wordwrap(ctx, 32, 44 + (11 * 8), 320-64,
                        "* Start - return to main menu\n");
                    break;
                case 2:
                    text_draw_wordwrap(ctx, 32, 44, 320-64,
                        "User can take one or more measurements of the "
                        "analog values. More measurements help even out "
                        "any variations caused either by user error or "
                        "stick inconsistency. A median is taken from "
                        "all measurements and gets displayed in light blue "
                        "as the default measurement. The remaining "
                        "measurements are drawn in the background in gray "
                        "to visualise deviations.\n\n"

                        "Optionally, a comparison to an example range "
                        "(displayed in green) can be enabled, which helps "
                        "to judge the controller's range."
                    );
                    break;
                case 3:
                    text_draw_wordwrap(ctx, 32, 44, 320-64,
                        "The measurement display can also be overriden with "
                        "one of the example ones, to let user view the expected "
                        "values and angles. The measurement is displayed in pink "
                        "to differentiate between actual measurements.\n\n"

                        "Some particularly bad controllers can have overly "
                        "high range, which would not fit the screen. Zoom "
                        "will be automatically changed to 75\% to compensate "
                        "in those cases, but the setting can be manually "
                        "overriden by user."
                    );
                    break;
                case 4:
                    text_draw_wordwrap(ctx, 32, 44, 320-64,
                        "Absolute analog values for each notch are displayed "
                        "on the right of the screen, as well as angles "
                        "for each diagonal. The values have colors assigned "
                        "based on following criteria:\n"
                        "* green - 80+ magnitude OR w/in 1" SYMBOL_DEGREES " from 45" SYMBOL_DEGREES "\n"
                        "* lime - 75+ magnitude OR w/in 3" SYMBOL_DEGREES " from 45" SYMBOL_DEGREES "\n"
                        "* orange - 70+ magnitude OR w/in 5" SYMBOL_DEGREES " from 45" SYMBOL_DEGREES "\n"
                        "* red - any other value\n\n"

                        "The diagonal magnitude cutoffs are 9/8th times "
                        "the cardinal ones to compensate for higher "
                        "magnitude diagonals on original N64 controllers."
                    );
                    break;
                case 5:
                    text_draw_wordwrap(ctx, 32, 44, 320-64, 
                        "Displays live X/Y values on a graph using ideal "
                        "OEM or Hori values as an overlay. Displays "
                        "the most recent 1024 values in blue, and the "
                        "current X/Y values as integers.\n\n"
                    );
                    break;
                case 6:
                    text_draw_wordwrap(ctx, 32, 44, 320-64, 
                        "Displays live X/Y values on an oscilloscope-style "
                        "display. Useful for identifying skips and "
                        "snapback issues.\n\n"
                    );
                    break;
                }


                display_show(ctx);

                controller_scan();
                struct controller_data cdata = get_keys_down_filtered();
                
                if (cdata.c[0].A || cdata.c[0].B || cdata.c[0].start) {
                    current_screen = SCR_MAIN_MENU;
                    break;
                }

                if (cdata.c[0].right || cdata.c[0].R) {
                    if (page < pages - 1) page++;
                }

                if (cdata.c[0].left || cdata.c[0].L) {
                    if (page > 0) page--;
                }
            }
            break;
        case SCR_RANGE_TEST:
            if (is_unsaved_result) {
                for (;;) {
                    while ((ctx = display_lock()) == 0) {}

                    graphics_fill_screen(ctx, COLOR_BACKGROUND);

                    graphics_set_color(COLOR_FOREGROUND, 0);
                    text_set_font(FONT_BOLD);
                    text_draw(ctx, 160, 80, "Previous result will be discarded.\nAre you sure?", ALIGN_CENTER);
                    text_set_font(FONT_MEDIUM);
                    text_draw(ctx, 160, 160, "Press Start to continue or B to cancel.", ALIGN_CENTER);

                    display_show(ctx);

                    controller_scan();
                    struct controller_data cdata = get_keys_down_filtered();

                    if (cdata.c[0].start) {
                        is_unsaved_result = 0;
                        break;
                    } else if (cdata.c[0].B) {
                        break;
                    }
                }
            }
            if (is_unsaved_result) {
                current_screen = SCR_MAIN_MENU;
                break;
            }

            for (int i = 0; i < sample_count; i++) {
                test_angles(&result[i], i+1);
            }
            is_unsaved_result = 1;
            current_screen = SCR_RANGE_RESULT;
            break;
        case SCR_RANGE_RESULT:
            display_angles(result, sample_count);
            current_screen = SCR_MAIN_MENU;
        case SCR_LIVE:
            display_live_ranges();
            current_screen = SCR_MAIN_MENU;
        case SCR_OSCOPE:
            display_oscilloscope();
            current_screen = SCR_MAIN_MENU;
        }
    }
}
