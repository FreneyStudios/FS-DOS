#include "gui/window.h"
#include "C/string.h"
#include "gfx/2d_gfx.h"

static window_t g_windows[MAX_WINDOWS];

void init_window_manager() {
    memset(g_windows, 0, sizeof(window_t) * MAX_WINDOWS);
}

window_t *create_window(int32_t x, int32_t y, int32_t width, int32_t height, const char *title) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (!g_windows[i].visible) {
            window_t *win = &g_windows[i];
            win->x = x;
            win->y = y;
            win->width = width;
            win->height = height;
            strncpy((uint8_t*)win->title, (uint8_t*)title, MAX_WINDOW_TITLE - 1);
            win->title[MAX_WINDOW_TITLE - 1] = '\0';
            win->visible = true;
            win->active = true;
            return win;
        }
    }
    return NULL; // No available window slots
}

void draw_window(window_t *window) {
    if (!window || !window->visible) {
        return;
    }

    Point top_left = {window->x, window->y};
    Point bottom_right = {window->x + window->width, window->y + window->height};

    // Draw window background
    fill_rect_solid(top_left, bottom_right, convert_color(LIGHT_GRAY));

    // Draw window border
    draw_rect(top_left, bottom_right, convert_color(BLACK));

    // Draw title bar
    Point title_bar_bottom_right = {bottom_right.X, top_left.Y + 20};
    fill_rect_solid(top_left, title_bar_bottom_right, convert_color(DARK_GRAY));

    // Draw title text
    // TODO: Need a printf-like function that can draw to a specific location
    // For now, we'll skip drawing the title text.
}

void draw_all_windows() {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        draw_window(&g_windows[i]);
    }
}
