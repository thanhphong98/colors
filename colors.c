#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#include "colors.h"

#define RGB565(rgb888)      ( ((((rgb888&0xFF0000)>>16) & 0xF8) << 8) |  \
                              ((((rgb888&0x00FF00)>>8) & 0xFC) << 3) |  \
                              ((((rgb888&0x0000FF)>>0) & 0xF8) >> 3) )

static float max(float a, float b, float c) {
    return ((a > b) ? (a > c ? a : c) : (b > c ? b : c));
}

static float min(float a, float b, float c) {
    return ((a < b) ? (a < c ? a : c) : (b < c ? b : c));
}

void rgb_2_hsv(uint32_t rgb, color_hsv_t *hsv)
{
	float h = 0, s, v;
    float r = (float)((uint8_t)(rgb >> 16)) / 255.0;
    float g = (float)((uint8_t)(rgb >> 8)) / 255.0;
    float b = (float)((uint8_t)(rgb >> 0)) / 255.0;
    float cmax = max(r, g, b);     // maximum of r, g, b
    float cmin = min(r, g, b);     // minimum of r, g, b
    float diff = cmax - cmin;      // diff of cmax and cmin.
    if (cmax == cmin)
        h = 0;
    else if (cmax == r)
        h = fmod((60.0 * ((g - b) / diff) + 360.0), 360.0);
    else if (cmax == g)
        h = fmod((60.0 * ((b - r) / diff) + 120.0), 360.0);
    else if (cmax == b)
        h = fmod((60.0 * ((r - g) / diff) + 240.0), 360.0);
    // if cmax equal zero
    if (cmax == 0)
        s = 0;
    else
        s = (diff / cmax) * 100.0;
    // compute v
    v = cmax * 100.0;

    hsv->hue = (uint32_t)h;
    hsv->sat = (uint32_t)s;
    hsv->val = (uint32_t)v;
}

void hsv_2_rgb(color_hsv_t hsv, uint32_t *rgb) {
    float r, g, b;
    hsv.hue %= 360;     // h -> [0,360]
    float rgb_max = (float)hsv.val * 2.55f;
    float rgb_min = rgb_max * (100.0 - (float)hsv.sat) / 100.0f;
    uint32_t i = hsv.hue / 60;
    uint32_t diff = hsv.hue % 60;

    // RGB adjustment amount by hue
    float rgb_adj = (rgb_max - rgb_min) * (float)diff / 60.0;
    switch (i) {
    case 0:
        r = rgb_max;
        g = rgb_min + rgb_adj;
        b = rgb_min;
        break;
    case 1:
        r = rgb_max - rgb_adj;
        g = rgb_max;
        b = rgb_min;
        break;
    case 2:
        r = rgb_min;
        g = rgb_max;
        b = rgb_min + rgb_adj;
        break;
    case 3:
        r = rgb_min;
        g = rgb_max - rgb_adj;
        b = rgb_max;
        break;
    case 4:
        r = rgb_min + rgb_adj;
        g = rgb_min;
        b = rgb_max;
        break;
    default:
        r = rgb_max;
        g = rgb_min;
        b = rgb_max - rgb_adj;
        break;
    }
    uint32_t _r = (uint32_t)r;
    uint32_t _g = (uint32_t)g;
    uint32_t _b = (uint32_t)b;
    *rgb = ((_r << 16) + (_g << 8) + _b);
}

void rgb_2_color565(uint32_t rgb, uint16_t *color565)
{
    *color565 = RGB565(rgb);
}

void color565_2_rgb(uint16_t color565, uint32_t *rgb)
{
    uint32_t r = ((((color565 >> 11) & 0x1F) * 527) + 23) >> 6;
    uint32_t g = ((((color565 >> 5) & 0x3F) * 259) + 33) >> 6;
    uint32_t b = (((color565 & 0x1F) * 527) + 23) >> 6;

    *rgb = ((r << 16) + (g << 8) + b);
}

void rgb_2_swap565(uint32_t rgb, uint16_t *swap565)
{
    uint16_t color_565 = RGB565(rgb);
    *swap565 = ((color_565 << 8) & 0xFF00) | ((color_565 >> 8) & 0x00FF);
}