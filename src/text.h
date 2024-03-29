#pragma once

#include <libdragon.h>

#define SYMBOL_DEGREES "\x1f"

enum Font
{
    FONT_MEDIUM = 0,
    FONT_BOLD = 1,
};

enum TextAlign
{
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_CENTER,
};

void text_init();
void text_set_font(enum Font f);
void text_set_line_height(int h);
int text_get_line_width(const char s[]);
void text_draw(display_context_t ctx, int x, int y, const char s[], enum TextAlign align);
void text_draw_wordwrap(display_context_t ctx, int x, int y, int w, const char s[]);
