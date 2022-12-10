#include <stdlib.h>

#include "text.h"

struct FontData
{
    sprite_t *sprite;
    uint8_t char_widths[128];
};

struct FontData fonts[2];
enum Font current_font = 0;

void load_font(struct FontData *fontdata, const char sprite_path[], const char charwidth_path[])
{
    int f = dfs_open(sprite_path);
    int size = dfs_size(f);
    sprite_t *sprite = malloc(size);
    dfs_read(sprite, size, 1, f);
    fontdata->sprite = sprite;
    dfs_close(f);

    f = dfs_open(charwidth_path);
    dfs_read(fontdata->char_widths, sizeof(fontdata->char_widths), 1, f);
    dfs_close(f);
}

void text_init()
{
    load_font(&fonts[FONT_MEDIUM], "font/medium.sprite", "font/medium_charwidths.bin");
    load_font(&fonts[FONT_BOLD], "font/bold.sprite", "font/bold_charwidths.bin");
    text_set_font(FONT_MEDIUM);
}

void text_set_font(enum Font f)
{
    current_font = f;
    graphics_set_font_sprite(fonts[f].sprite);
}

int text_get_line_width(const char s[])
{
    int w = 0;
    for (; *s != 0 && *s != '\n'; s++) {
        w += fonts[current_font].char_widths[(int)*s];
    }

    return w;
}

void text_draw(display_context_t ctx, int x, int y, const char s[], enum TextAlign align)
{
    int x_init = x;

reposition:
    int w = text_get_line_width(s);
    if (align == ALIGN_RIGHT) {
        x -= w;
    }
    if (align == ALIGN_CENTER) {
        x -= w / 2;
    }

    for (; *s != 0; s++) {
        switch (*s)
        {
        case '\n':
            x = x_init;
            y += 10;
            s++;
            goto reposition;
            break;
        case ' ':
            x += fonts[current_font].char_widths[(int)*s];
            break;
        default:
            graphics_draw_character(ctx, x, y, *s);
            x += fonts[current_font].char_widths[(int)*s];
            break;
        }
    }
}
