#include "lvgl/lvgl.h"

/*******************************************************************************
 * Size: 48 px
 * Bpp: 2
 * Opts: 
 ******************************************************************************/

#ifndef PONG_48
#define PONG_48 1
#endif

#if PONG_48

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t gylph_bitmap[] = {
    /* U+E900 "" */
    0x0, 0x0, 0x54, 0x0, 0x0, 0x0, 0xb, 0xff,
    0xd0, 0x0, 0x0, 0x3e, 0x39, 0xf4, 0x0, 0x0,
    0xff, 0xbe, 0xfd, 0x0, 0x2, 0xff, 0xff, 0xff,
    0x0, 0x7, 0x8e, 0x38, 0xe3, 0xc0, 0xf, 0xef,
    0xbe, 0xfb, 0xd0, 0x1f, 0xff, 0xff, 0xff, 0xf0,
    0x2f, 0x8e, 0x38, 0xe3, 0xf0, 0x3f, 0xef, 0xbe,
    0xfb, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xfc, 0xb3,
    0x8e, 0x38, 0xe3, 0xac, 0xbb, 0xef, 0xbe, 0xfb,
    0xec, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xe3, 0x8e,
    0x38, 0xe3, 0x8d, 0xfb, 0xef, 0xbe, 0xfb, 0xed,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xe3, 0x8e, 0x38,
    0xe3, 0x8d, 0xfb, 0xef, 0xbe, 0xfb, 0xed, 0xbf,
    0xff, 0xff, 0xff, 0xfc, 0x77, 0x8e, 0x38, 0xe3,
    0xbc, 0x3f, 0xef, 0xbe, 0xfb, 0xf4, 0x1f, 0xff,
    0xff, 0xff, 0xf0, 0x7, 0xde, 0x38, 0xeb, 0xc0,
    0x0, 0xff, 0xbe, 0xfe, 0x0, 0x0, 0x1b, 0xff,
    0x90, 0x0, 0x0, 0x0, 0x38, 0x0, 0x0, 0x0,
    0x0, 0x38, 0x0, 0x0, 0x0, 0x0, 0x38, 0x0,
    0x0, 0x0, 0x0, 0x38, 0x0, 0x0, 0x0, 0x0,
    0x38, 0x0, 0x0, 0x0, 0x0, 0x38, 0x0, 0x0,
    0x0, 0x0, 0x38, 0x0, 0x0, 0x0, 0x0, 0x38,
    0x0, 0x0, 0x0, 0x0, 0xfe, 0x0, 0x0, 0x0,
    0x1, 0xff, 0x0, 0x0, 0x0, 0x2, 0xff, 0x0,
    0x0, 0x0, 0x2, 0xff, 0x0, 0x0, 0x0, 0x2,
    0xff, 0x0, 0x0, 0x0, 0x2, 0xff, 0x0, 0x0,
    0x0, 0x2, 0xff, 0x0, 0x0, 0x0, 0x2, 0xff,
    0x0, 0x0, 0x0, 0x2, 0xff, 0x0, 0x0, 0x0,
    0x1, 0xff, 0x0, 0x0, 0x0, 0x0, 0xfe, 0x0,
    0x0, 0x0, 0x0, 0x14, 0x0, 0x0,

    /* U+E901 "" */
    0x0, 0x0, 0x0, 0x2f, 0x80, 0xf, 0xff, 0x2,
    0xff, 0xf8, 0x3f, 0xff, 0xc3, 0xff, 0xfc, 0x2f,
    0xff, 0x80, 0xff, 0xf0, 0x2, 0xf8, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x2f, 0x80, 0xf, 0xff, 0x2, 0xff, 0xf8,
    0x3f, 0xff, 0xc3, 0xff, 0xfc, 0x2f, 0xff, 0x80,
    0xff, 0xf0, 0x2, 0xf8, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2f,
    0x80, 0xf, 0xff, 0x2, 0xff, 0xf8, 0x3f, 0xff,
    0xc3, 0xff, 0xfc, 0x2f, 0xff, 0x80, 0xff, 0xf0,
    0x2, 0xf8, 0x0, 0x0, 0x0,

    /* U+E902 "" */
    0x0, 0x0, 0x5, 0x95, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x2f, 0xff, 0xfe, 0x0, 0x0, 0x0, 0x0,
    0x1f, 0xff, 0xff, 0xfe, 0x0, 0x0, 0x0, 0xb,
    0xff, 0xff, 0xff, 0xfc, 0x0, 0x0, 0x2, 0xff,
    0xff, 0xff, 0xff, 0xf0, 0x0, 0x0, 0x7f, 0xff,
    0xff, 0xff, 0xff, 0x80, 0x0, 0xf, 0xff, 0xff,
    0xff, 0xff, 0xfc, 0x0, 0x1, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xe0, 0x0, 0x3f, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x0, 0x3, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xf4, 0x0, 0x3f, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x40, 0x7, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xf8, 0x0, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x80, 0x7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8,
    0x0, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80,
    0x3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf4, 0x0,
    0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5,
    0x55, 0x55, 0x55, 0x55, 0x54, 0x0, 0x0, 0xbf,
    0xff, 0xff, 0xff, 0xff, 0xc0, 0x0, 0x3, 0xff,
    0xff, 0xff, 0xff, 0xf0, 0x0, 0x0, 0xf, 0xff,
    0xff, 0xff, 0xfc, 0x0, 0x0, 0x0, 0x2f, 0xff,
    0xff, 0xfe, 0x0, 0x29, 0x0, 0x0, 0x2f, 0xff,
    0xff, 0x40, 0xf, 0xf8, 0x0, 0x0, 0x5, 0x55,
    0x0, 0x2, 0xff, 0xc0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x3f, 0xfc, 0x0, 0x0, 0x3f, 0xff, 0x0,
    0x2, 0xff, 0xc0, 0x0, 0x3, 0xff, 0xf0, 0x0,
    0xb, 0xf0, 0x0, 0x0, 0x3f, 0xff, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x3, 0xff, 0xf0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x3f, 0xff, 0x40, 0x0, 0x0,
    0x0, 0x0, 0x3, 0xff, 0xf4, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x3f, 0xff, 0x40, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x6a, 0x50, 0x0, 0x0, 0x0,

    /* U+E903 "" */
    0x0, 0x1b, 0xfe, 0x40, 0x0, 0xf, 0xff, 0xff,
    0x0, 0x7, 0xff, 0xff, 0xfd, 0x0, 0xff, 0xff,
    0xff, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xc7, 0xff,
    0xff, 0xff, 0xfd, 0xbf, 0xff, 0xff, 0xff, 0xef,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xfb, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff,
    0xff, 0xff, 0xd3, 0xff, 0xff, 0xff, 0xfc, 0xf,
    0xff, 0xff, 0xff, 0x0, 0x7f, 0xff, 0xff, 0xd0,
    0x0, 0xff, 0xff, 0xf0, 0x0, 0x1, 0xbf, 0xe4,
    0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 768, .box_w = 20, .box_h = 46, .ofs_x = 15, .ofs_y = -2},
    {.bitmap_index = 230, .adv_w = 768, .box_w = 10, .box_h = 34, .ofs_x = 19, .ofs_y = 3},
    {.bitmap_index = 315, .adv_w = 768, .box_w = 30, .box_h = 34, .ofs_x = 10, .ofs_y = 4},
    {.bitmap_index = 570, .adv_w = 768, .box_w = 18, .box_h = 18, .ofs_x = 15, .ofs_y = 12}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 59648, .range_length = 4, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

/*Store all the custom data of the font*/
static lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = gylph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 2,
    .kern_classes = 0,
    .bitmap_format = 0
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
lv_font_t pong_48 = {
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 46,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = 0,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if PONG_48*/

