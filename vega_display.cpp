/*
    Copyright 2018 Jan Pomikalek <jan.pomikalek@gmail.com>

    This file is part of the VeGa firmware.

    VeGa firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VeGa firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VeGa firmware.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "vesc_comm.h"
#include <TFT_22_ILI9225.h>

// pins
#define TFT_RST 12
#define TFT_RS  9
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 0   // 0 if wired to +5V directly

#define TFT_BRIGHTNESS 200  // irrelevant with the 2.0" 176x220 ILI9225 display

#define BATTERY_INDICATOR_CELL_WIDTH 14
#define BATTERY_INDICATOR_CELL_HEIGHT 15

#define SPEED_INDICATOR_CELL_WIDTH 10
#define SPEED_INDICATOR_CELL_HEIGHT 11

#define LEN(X) (sizeof(X) / sizeof(X[0]))

typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

const Point PROGMEM BATTERY_INDICATOR_CELLS[] = {
    // left column
    {0, 204},  {0, 187},  {0, 170},  {0, 153},  {0, 136},
    {0, 119},  {0, 102},  {0, 85},  {0, 68},  {0, 51},
    {0, 34},  {0, 17},
    // top row
    {0, 0},  {16, 0},  {32, 0},  {48, 0},  {64, 0},
    {80, 0},  {96, 0},  {112, 0},  {128, 0},  {144, 0},
    {160, 0},
    // right column
    {160, 17},  {160, 34},  {160, 51},  {160, 68},  {160, 85},
    {160, 102},  {160, 119},  {160, 136},  {160, 153},  {160, 170},
    {160, 187},  {160, 204},
};

const Point PROGMEM SPEED_INDICATOR_CELLS[] = {
    // bottom-left row
    {57, 151},  {41, 151}, {25, 151},
    // left column
    {25, 134}, {25, 117}, {25, 100}, {25, 84},
    // top row
    {25, 66}, {41, 66}, {57, 66}, {73, 66}, {90, 66},
    {106, 66}, {122, 66}, {138, 66},
    // right column
    {138, 83}, {138, 100}, {138, 117}, {138, 134},
    // bottom-right row
    {138, 151}, {122, 151}, {106, 151},
};

// TODO: PROGMEM
const bool FONT_DIGITS_3x5[10][5][3] = {
    {
        {1, 1, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
    },
    {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
    },
    {
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1},
        {1, 0, 0},
        {1, 1, 1},
    },
    {
        {1, 1, 1},
        {0, 0, 1},
        {0, 1, 1},
        {0, 0, 1},
        {1, 1, 1},
    },
    {
        {1, 0, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 0, 1},
        {0, 0, 1},
    },
    {
        {1, 1, 1},
        {1, 0, 0},
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1},
    },
    {
        {1, 1, 1},
        {1, 0, 0},
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
    },
    {
        {1, 1, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
    },
    {
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
    },
    {
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
        {0, 0, 1},
        {1, 1, 1},
    }
};

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

// Remember how many cells are currently filled so that we can update the indicators more efficiently.
uint8_t battery_cells_filled = 0;
uint8_t speed_cells_filled = 0;

void display_init() {
    tft.begin();

    tft.setOrientation(0);
    tft.fillRectangle(0, 0, tft.maxX() - 1, tft.maxY() - 1, COLOR_BLACK);

    tft.setBackgroundColor(COLOR_BLACK);
}

void
display_draw_digit(uint8_t digit, uint8_t x, uint8_t y, uint16_t fg_color, uint16_t bg_color, uint8_t magnify = 1) {
    for (int xx = 0; xx < 3; xx++) {
        for (int yy = 0; yy < 5; yy++) {
            uint16_t color = FONT_DIGITS_3x5[digit][yy][xx] ? fg_color : bg_color;
            int x1 = x + xx * magnify;
            int y1 = y + yy * magnify;
            tft.fillRectangle(x1, y1, x1 + magnify - 1, y1 + magnify - 1, color);
        }
    }
}

void
display_draw_number(char *number, uint8_t x, uint8_t y, uint16_t fg_color, uint16_t bg_color, uint8_t spacing, uint8_t magnify = 1) {
    int cursor_x = x;
    int number_len = strlen(number);
    for (int i=0; i < number_len; i++) {
        char ch = number[i];
        if (ch >= '0' and ch <= '9') {
            display_draw_digit(ch - '0', cursor_x, y, fg_color, bg_color, magnify);
            cursor_x += 3 * magnify + spacing;
        } else if (ch == '.') {
            tft.fillRectangle(cursor_x, y, cursor_x + magnify - 1, y + 5 * magnify - 1, bg_color);
            tft.fillRectangle(cursor_x, y + 4 * magnify, cursor_x + magnify - 1, y + 5 * magnify - 1, fg_color);
            cursor_x += magnify + spacing;
        } else if (ch == ' ') {
            tft.fillRectangle(cursor_x, y, cursor_x + 3 * magnify - 1, y + 5 * magnify - 1, bg_color);
            cursor_x += 3 * magnify + spacing;
        }
    }
}

void display_draw_labels() {
    tft.setFont(Terminal6x8);

    tft.drawText(36, 48, "VOLTS", COLOR_WHITE);
    tft.drawText(132, 48, "MAH", COLOR_WHITE);

    tft.drawText(90, 131, "KPH", COLOR_WHITE);

    tft.drawText(23, 175, "TRIP", COLOR_WHITE);
    tft.drawText(97, 175, "TOTAL", COLOR_WHITE);
    tft.drawText(70, 208, "KM", COLOR_WHITE);
    tft.drawText(139, 208, "KM", COLOR_WHITE);
}

void display_set_volts(float volts) {
    char fmt[5];
    dtostrf(volts, 4, 1, fmt);
    display_draw_number(fmt, 24, 25, COLOR_WHITE, COLOR_BLACK, 2, 4);
}

void display_set_mah(uint16_t mah) {
    char fmt[6];
    dtostrf(mah, 5, 0, fmt);
    display_draw_number(fmt, 84, 25, COLOR_WHITE, COLOR_BLACK, 2, 4);
}

void display_set_trip_distance(uint32_t meters, uint16_t color = COLOR_WHITE) {
    char fmt[7];
    dtostrf(meters / 1000.0, 5, 2, fmt);
    display_draw_number(fmt, 24, 185, color, COLOR_BLACK, 2, 4);
}

void display_set_total_distance(uint32_t meters) {
    char fmt[6];
    dtostrf(meters / 1000.0, 4, 0, fmt);
    display_draw_number(fmt, 98, 185, COLOR_WHITE, COLOR_BLACK, 2, 4);
}

void display_set_speed(uint8_t kph) {
    if (kph >= 10)
        display_draw_digit(kph / 10, 60, 91, COLOR_WHITE, COLOR_BLACK, 7);
    else
        tft.fillRectangle(60, 91, 82, 127, COLOR_BLACK);
    display_draw_digit(kph % 10, 89, 91, COLOR_WHITE, COLOR_BLACK, 7);
}

bool draw_battery_cell(int index, bool filled, bool redraw = false) {
    uint16_t p_word = pgm_read_word_near(BATTERY_INDICATOR_CELLS + index);
    Point *p = (Point *) &p_word;
    if (filled || redraw) {
        uint8_t cell_count = LEN(BATTERY_INDICATOR_CELLS);
        uint8_t green = (uint8_t)(255.0 / (cell_count - 1) * index);
        uint8_t red = 255 - green;
        uint16_t color = tft.setColor(red, green, 0);
        tft.fillRectangle(
                p->x, p->y,
                p->x + BATTERY_INDICATOR_CELL_WIDTH, p->y + BATTERY_INDICATOR_CELL_HEIGHT,
                color
        );
    }
    if (!filled) {
        tft.fillRectangle(
                p->x + 1, p->y + 1,
                p->x + BATTERY_INDICATOR_CELL_WIDTH - 1, p->y + BATTERY_INDICATOR_CELL_HEIGHT - 1,
                COLOR_BLACK
        );
    }
}

void display_update_battery_indicator(float battery_percent, bool redraw = false) {
    int cells_to_fill = round(battery_percent * LEN(BATTERY_INDICATOR_CELLS));
    if (redraw) {
        for (int i = 0; i < LEN(BATTERY_INDICATOR_CELLS); i++)
            draw_battery_cell(i, i < cells_to_fill, true);
    }
    else {
        if (cells_to_fill > battery_cells_filled) {
            for (int i = battery_cells_filled; i < cells_to_fill; i++)
                draw_battery_cell(i, true);
        }
        if (cells_to_fill < battery_cells_filled) {
            for (int i = battery_cells_filled - 1; i >= cells_to_fill; i--)
                draw_battery_cell(i, false);
        }
    }
    battery_cells_filled = cells_to_fill;
}

bool draw_speed_cell(int index, bool filled, bool redraw = false) {
    uint16_t p_word = pgm_read_word_near(SPEED_INDICATOR_CELLS + index);
    Point *p = (Point *) &p_word;
    if (filled || redraw) {
        tft.fillRectangle(
                p->x, p->y,
                p->x + SPEED_INDICATOR_CELL_WIDTH, p->y + SPEED_INDICATOR_CELL_HEIGHT,
                tft.setColor(150, 150, 255)
        );
    }
    if (!filled) {
        tft.fillRectangle(
                p->x + 1, p->y + 1,
                p->x + SPEED_INDICATOR_CELL_WIDTH - 1, p->y + SPEED_INDICATOR_CELL_HEIGHT - 1,
                COLOR_BLACK
        );
    }
}

void display_update_speed_indicator(float speed_percent, bool redraw = false) {
    int cells_to_fill = round(speed_percent * LEN(SPEED_INDICATOR_CELLS));
    if (redraw) {
        for (int i = 0; i < LEN(SPEED_INDICATOR_CELLS); i++)
            draw_speed_cell(i, i < cells_to_fill, true);
    }
    else {
        if (cells_to_fill > speed_cells_filled) {
            for (int i = speed_cells_filled; i < cells_to_fill; i++)
                draw_speed_cell(i, true);
        }
        if (cells_to_fill < speed_cells_filled) {
            for (int i = speed_cells_filled - 1; i >= cells_to_fill; i--)
                draw_speed_cell(i, false);
        }
    }
    speed_cells_filled = cells_to_fill;
}

void display_indicate_read_success(uint32_t duration_ms) {
    tft.fillRectangle(85, 155, 89, 159, tft.setColor(0, 150, 0));
    delay(duration_ms);
    tft.fillRectangle(85, 155, 89, 159, COLOR_BLACK);
}

void display_indicate_read_failure(uint32_t duration_ms) {
    tft.fillRectangle(85, 155, 89, 159, tft.setColor(150, 0, 0));
    delay(duration_ms);
    tft.fillRectangle(85, 155, 89, 159, COLOR_BLACK);
}

uint16_t display_make_color(uint8_t red, uint8_t green, uint8_t blue) {
    return tft.setColor(red, green, blue);
}
