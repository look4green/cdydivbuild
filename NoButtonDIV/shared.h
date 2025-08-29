// shared.h
#ifndef SHARED_H
#define SHARED_H

#include <Arduino.h>

// Consistent Color Definitions
const uint16_t BLACK = 0x0000;
const uint16_t WHITE = 0xFFFF;
const uint16_t DARK_GRAY = 0x4208;
const uint16_t GRAY = 0x8410;
const uint16_t LIGHT_GRAY = 0xC618;
const uint16_t RED = 0xF800;
const uint16_t ORANGE = 0xFB20; // A more vibrant orange
const uint16_t YELLOW = 0xFFE0;
const uint16_t GREEN = 0x07E0;
const uint16_t BLUE = 0x001F;
const uint16_t DARK_BLUE = 0x000F;
const uint16_t CYAN = 0x07FF;
const uint16_t MAGENTA = 0xF81F;

#define SELECTED_ICON_COLOR ORANGE

// Global state variables
extern bool in_sub_menu;
extern bool feature_active;
extern bool submenu_initialized;
extern bool is_main_menu;
extern bool feature_exit_requested;

#endif // SHARED_H