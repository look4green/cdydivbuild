#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Wire.h>

#include "utils.h"
#include "shared.h"
#include "icon.h"

TFT_eSPI tft = TFT_eSPI();

bool feature_active = false;
bool feature_exit_requested = false;

const int NUM_MENU_ITEMS = 8;
const char *menu_items[NUM_MENU_ITEMS] = {
    "WiFi",
    "Bluetooth",
    "2.4GHz",
    "SubGHz",
    "IR Remote",
    "Tools",
    "Setting",
    "About"
};

const unsigned char *bitmap_icons[NUM_MENU_ITEMS] = {
    bitmap_icon_wifi,
    bitmap_icon_spoofer,
    bitmap_icon_jammer,
    bitmap_icon_analyzer,
    bitmap_icon_led,
    bitmap_icon_stat,
    bitmap_icon_setting,
    bitmap_icon_question
};

int current_menu_index = 0;
bool is_main_menu = false;

const int NUM_SUBMENU_ITEMS = 7;
const char *submenu_items[NUM_SUBMENU_ITEMS] = {
    "Packet Monitor",
    "Beacon Spammer",
    "WiFi Deauther",
    "Deauth Detector",
    "WiFi Scanner",
    "Captive Portal",
    "Back to Main Menu"
};

const int bluetooth_NUM_SUBMENU_ITEMS = 6;
const char *bluetooth_submenu_items[bluetooth_NUM_SUBMENU_ITEMS] = {
    "BLE Jammer",
    "BLE Spoofer",
    "Sour Apple",
    "Sniffer",
    "BLE Scanner",
    "Back to Main Menu"
};

const int nrf_NUM_SUBMENU_ITEMS = 5;
const char *nrf_submenu_items[nrf_NUM_SUBMENU_ITEMS] = {
    "Scanner",
    "Analyzer [Coming soon]",
    "WLAN Jammer [Coming soon]",
    "Proto Kill",
    "Back to Main Menu"
};

const int subghz_NUM_SUBMENU_ITEMS = 5;
const char *subghz_submenu_items[subghz_NUM_SUBMENU_ITEMS] = {
    "Replay Attack",
    "Bruteforce [Coming soon]",
    "SubGHz Jammer",
    "Saved Profile",
    "Back to Main Menu"
};

const int tools_NUM_SUBMENU_ITEMS = 3;
const char *tools_submenu_items[tools_NUM_SUBMENU_ITEMS] = {
    "Serial Monitor",
    "Update Firmware",
    "Back to Main Menu"
};

const int ir_NUM_SUBMENU_ITEMS = 3;
const char *ir_submenu_items[ir_NUM_SUBMENU_ITEMS] = {
    "Record [Coming soon]",
    "Saved Profile [Coming soon]",
    "Back to Main Menu"
};

const int about_NUM_SUBMENU_ITEMS = 1;
const char *about_submenu_items[about_NUM_SUBMENU_ITEMS] = {
    "Back to Main Menu"
};

int current_submenu_index = 0;
bool in_sub_menu = false;

const char **active_submenu_items = nullptr;
int active_submenu_size = 0;

const unsigned char *wifi_submenu_icons[NUM_SUBMENU_ITEMS] = {
    bitmap_icon_wifi,      // Packet Monitor
    bitmap_icon_antenna,   // Beacon Spammer
    bitmap_icon_wifi_jammer,  // WiFi Deauther
    bitmap_icon_eye2,      // Deauth Detector
    bitmap_icon_jammer,      // WiFi Scanner
    bitmap_icon_bash,      // Captive Portal
    bitmap_icon_go_back
};

const unsigned char *bluetooth_submenu_icons[bluetooth_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_ble_jammer,   // BLE Jammer
    bitmap_icon_spoofer,      // BLE Spoofer
    bitmap_icon_apple,      // Sour Apple
    bitmap_icon_analyzer,    // Analyzer
    bitmap_icon_graph,      // BLE Scanner
    bitmap_icon_go_back
};

const unsigned char *nrf_submenu_icons[nrf_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_scanner,      // Scanner
    bitmap_icon_question,
    bitmap_icon_question,
    bitmap_icon_kill,      // Proto Kill
    bitmap_icon_go_back
};

const unsigned char *subghz_submenu_icons[subghz_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_antenna,  // Replay Attack
    bitmap_icon_question,
    bitmap_icon_no_signal, // SubGHz Jammer
    bitmap_icon_list,      // Saved Profile
    bitmap_icon_go_back
};

const unsigned char *tools_submenu_icons[tools_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_bash,    // Serial Monitor
    bitmap_icon_follow,  // Update Frimware
    bitmap_icon_go_back
};

const unsigned char *ir_submenu_icons[ir_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_question,
    bitmap_icon_question,
    bitmap_icon_go_back
};

const unsigned char *about_submenu_icons[about_NUM_SUBMENU_ITEMS] = {
    bitmap_icon_go_back
};

const unsigned char **active_submenu_icons = nullptr;

void updateActiveSubmenu() {
    switch (current_menu_index) {
        case 0: // WiFi
            active_submenu_items = submenu_items;
            active_submenu_size = NUM_SUBMENU_ITEMS;
            active_submenu_icons = wifi_submenu_icons;
            break;
        case 1: // Bluetooth
            active_submenu_items = bluetooth_submenu_items;
            active_submenu_size = bluetooth_NUM_SUBMENU_ITEMS;
            active_submenu_icons = bluetooth_submenu_icons;
            break;
        case 2: // 2.4GHz (NRF)
            active_submenu_items = nrf_submenu_items;
            active_submenu_size = nrf_NUM_SUBMENU_ITEMS;
            active_submenu_icons = nrf_submenu_icons;
            break;
        case 3: // SubGHz
            active_submenu_items = subghz_submenu_items;
            active_submenu_size = subghz_NUM_SUBMENU_ITEMS;
            active_submenu_icons = subghz_submenu_icons;
            break;
        case 4: // IR
            active_submenu_items = ir_submenu_items;
            active_submenu_size = ir_NUM_SUBMENU_ITEMS;
            active_submenu_icons = ir_submenu_icons;
            break;
        case 5: // Tools
            active_submenu_items = tools_submenu_items;
            active_submenu_size = tools_NUM_SUBMENU_ITEMS;
            active_submenu_icons = tools_submenu_icons;
            break;
        case 7:
            active_submenu_items = about_submenu_items;
            active_submenu_size = about_NUM_SUBMENU_ITEMS;
            active_submenu_icons = about_submenu_icons;
            break;
        default:
            active_submenu_items = nullptr;
            active_submenu_size = 0;
            active_submenu_icons = nullptr;
            break;
    }
}

float currentBatteryVoltage = readBatteryVoltage();
unsigned long last_interaction_time = 0;

int last_submenu_index = -1;
bool submenu_initialized = false;
int last_menu_index = -1;
bool menu_initialized = false;

// Universal function to handle all submenu button presses
void handleSubmenuButtons() {
    uint16_t x, y;
    if (tft.getTouch(&x, &y) && !feature_active) {
        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30;
            if (i == active_submenu_size - 1) {
                yPos += 10;
            }
            int button_x1 = 10;
            int button_y1 = yPos;
            int button_x2 = 110;
            int button_y2 = button_y1 + 25; 
            if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
                current_submenu_index = i;
                last_interaction_time = millis();
                displaySubmenu();
                delay(200);
                if (current_submenu_index == active_submenu_size - 1) {
                    in_sub_menu = false;
                    feature_active = false;
                    feature_exit_requested = false;
                    is_main_menu = true;
                    displayMenu();
                } else {
                    feature_active = true;
                    // Logic to start the selected feature goes here
                }
                break;
            }
        }
    }
}

void displaySubmenu() {
    menu_initialized = false;
    last_menu_index = -1;

    tft.setTextFont(2);
    tft.setTextSize(1);

    if (!submenu_initialized) {
        tft.fillScreen(TFT_BLACK);
        drawStatusBar(currentBatteryVoltage, false);

        for (int i = 0; i < active_submenu_size; i++) {
            int yPos = 30 + i * 30;
            if (i == active_submenu_size - 1) yPos += 10;

            tft.setTextColor((i == active_submenu_size - 1) ? WHITE : WHITE, BLACK);
            tft.drawBitmap(10, yPos, active_submenu_icons[i], 16, 16, (i == active_submenu_size - 1) ? WHITE : WHITE);
            tft.setCursor(30, yPos);
            if (i < active_submenu_size - 1) {
                tft.print("| ");
            }
            tft.print(active_submenu_items[i]);
        }
        submenu_initialized = true;
        last_submenu_index = -1;
    }
    if (last_submenu_index != current_submenu_index) {
        if (last_submenu_index >= 0) {
            int prev_yPos = 30 + last_submenu_index * 30;
            if (last_submenu_index == active_submenu_size - 1) prev_yPos += 10;
            tft.setTextColor((last_submenu_index == active_submenu_size - 1) ? WHITE : WHITE, BLACK);
            tft.drawBitmap(10, prev_yPos, active_submenu_icons[last_submenu_index], 16, 16, (last_submenu_index == active_submenu_size - 1) ? WHITE : WHITE);
            tft.setCursor(30, prev_yPos);
            if (last_submenu_index < active_submenu_size - 1) {
                tft.print("| ");
            }
            tft.print(active_submenu_items[last_submenu_index]);
        }
        int new_yPos = 30 + current_submenu_index * 30;
        if (current_submenu_index == active_submenu_size - 1) new_yPos += 10;
        tft.setTextColor((current_submenu_index == active_submenu_size - 1) ? ORANGE : ORANGE, BLACK);
        tft.drawBitmap(10, new_yPos, active_submenu_icons[current_submenu_index], 16, 16, (current_submenu_index == active_submenu_size - 1) ? ORANGE : ORANGE);
        tft.setCursor(30, new_yPos);
        if (current_submenu_index < active_submenu_size - 1) {
            tft.print("| ");
        }
        tft.print(active_submenu_items[current_submenu_index]);
        last_submenu_index = current_submenu_index;
    }
    drawStatusBar(currentBatteryVoltage, true);
}

const int COLUMN_WIDTH = 120;
const int X_OFFSET_LEFT = 10;
const int X_OFFSET_RIGHT = X_OFFSET_LEFT + COLUMN_WIDTH;
const int Y_START = 30;
const int Y_SPACING = 75;

void displayMenu() {
    const uint16_t icon_colors[NUM_MENU_ITEMS] = {
        WHITE, // WiFi
        WHITE, // Bluetooth
        WHITE, // 2.4GHz
        WHITE, // SubGHz
        WHITE, // IR Remote
        WHITE, // Tools
        GRAY, // Setting
        WHITE // About
    };
    submenu_initialized = false;
    last_submenu_index = -1;
    tft.setTextFont(2);

    if (!menu_initialized) {
        // Draw the main menu background color first
        tft.fillScreen(DARK_GRAY);
        // Then draw the status bar on top of the background
        drawStatusBar(currentBatteryVoltage, false);

        for (int i = 0; i < NUM_MENU_ITEMS; i++) {
            int column = i / 4;
            int row = i % 4;
            int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
            int y_position = Y_START + row * Y_SPACING;
            tft.fillRoundRect(x_position, y_position, 100, 60, 5, DARK_BLUE);
            tft.drawRoundRect(x_position, y_position, 100, 60, 5, GRAY);
            tft.drawBitmap(x_position + 42, y_position + 10, bitmap_icons[i], 16, 16, icon_colors[i]);
            tft.setTextColor(WHITE, DARK_BLUE);
            int textWidth = 6 * strlen(menu_items[i]);
            int textX = x_position + (100 - textWidth) / 2;
            int textY = y_position + 30;
            tft.setCursor(textX, textY);
            tft.print(menu_items[i]);
        }
        menu_initialized = true;
        last_menu_index = -1;
    }
    if (last_menu_index != current_menu_index) {
        for (int i = 0; i < NUM_MENU_ITEMS; i++) {
            int column = i / 4;
            int row = i % 4;
            int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
            int y_position = Y_START + row * Y_SPACING;
            if (i == last_menu_index) {
                tft.fillRoundRect(x_position, y_position, 100, 60, 5, DARK_BLUE);
                tft.drawRoundRect(x_position, y_position, 100, 60, 5, GRAY);
                tft.setTextColor(WHITE, DARK_BLUE);
                tft.drawBitmap(x_position + 42, y_position + 10, bitmap_icons[last_menu_index], 16, 16, icon_colors[last_menu_index]);
                int textWidth = 6 * strlen(menu_items[last_menu_index]);
                int textX = x_position + (100 - textWidth) / 2;
                int textY = y_position + 30;
                tft.setCursor(textX, textY);
                tft.print(menu_items[last_menu_index]);
            }
        }
        int column = current_menu_index / 4;
        int row = current_menu_index % 4;
        int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
        int y_position = Y_START + row * Y_SPACING;
        tft.fillRoundRect(x_position, y_position, 100, 60, 5, DARK_BLUE);
        tft.drawRoundRect(x_position, y_position, 100, 60, 5, ORANGE);
        tft.setTextColor(ORANGE, DARK_BLUE);
        tft.drawBitmap(x_position + 42, y_position + 10, bitmap_icons[current_menu_index], 16, 16, SELECTED_ICON_COLOR);
        int textWidth = 6 * strlen(menu_items[current_menu_index]);
        int textX = x_position + (100 - textWidth) / 2;
        int textY = y_position + 30;
        tft.setCursor(textX, textY);
        tft.print(menu_items[current_menu_index]);
        last_menu_index = current_menu_index;
    }
    drawStatusBar(currentBatteryVoltage, true);
}

void handleButtons() {
    if (in_sub_menu) {
        handleSubmenuButtons();
    } else {
        static unsigned long lastTouchTime = 0;
        const unsigned long touchFeedbackDelay = 100;
        uint16_t x, y;
        if (tft.getTouch(&x, &y) && !feature_active && (millis() - lastTouchTime >= touchFeedbackDelay)) {
            lastTouchTime = millis();
            for (int i = 0; i < NUM_MENU_ITEMS; i++) {
                int column = i / 4;
                int row = i % 4;
                int x_position = (column == 0) ? X_OFFSET_LEFT : X_OFFSET_RIGHT;
                int y_position = Y_START + row * Y_SPACING;
                int button_x1 = x_position;
                int button_y1 = y_position;
                int button_x2 = x_position + 100;
                int button_y2 = y_position + 60;
                if (x >= button_x1 && x <= button_x2 && y >= button_y1 && y <= button_y2) {
                    current_menu_index = i;
                    last_interaction_time = millis();
                    displayMenu();
                    unsigned long startTime = millis();
                    uint16_t touchX, touchY;
                    while (tft.getTouch(&touchX, &touchY) && (millis() - startTime < touchFeedbackDelay)) {
                        delay(10);
                    }
                    if (tft.getTouch(&touchX, &touchY)) {
                        updateActiveSubmenu();
                        if (active_submenu_items && active_submenu_size > 0) {
                            current_submenu_index = 0;
                            in_sub_menu = true;
                            submenu_initialized = false;
                            displaySubmenu();
                        } else {
                            if (is_main_menu) {
                                is_main_menu = false;
                                displayMenu();
                            } else {
                                is_main_menu = true;
                            }
                        }
                    }
                    delay(200);
                    break;
                }
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(BLACK);
    loading(100, ORANGE, 0, 0, 2, true);
    tft.fillScreen(BLACK);
    displayLogo(WHITE, 2000);
    displayMenu();
    drawStatusBar(currentBatteryVoltage, false);
    last_interaction_time = millis();
}

void loop() {
    handleButtons();
    updateStatusBar();
}