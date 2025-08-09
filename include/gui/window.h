#pragma once

#include "C/stdint.h"
#include "C/stdbool.h"
#include "gfx/2d_gfx.h"

#define MAX_WINDOWS 10
#define MAX_WINDOW_TITLE 256

typedef struct {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    char title[MAX_WINDOW_TITLE];
    bool active;
    bool visible;
} window_t;

void init_window_manager(void);
window_t *create_window(int32_t x, int32_t y, int32_t width, int32_t height, const char *title);
void draw_window(window_t *window);
void draw_all_windows(void);
