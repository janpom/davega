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
    {57, 145},  {41, 145}, {25, 145},
    // left column
    {25, 128}, {25, 111}, {25, 94}, {25, 77},
    // top row
    {25, 60}, {41, 60}, {57, 60}, {73, 60}, {90, 60},
    {106, 60}, {122, 60}, {138, 60},
    // right column
    {138, 77}, {138, 94}, {138, 111}, {138, 128},
    // bottom-right row
    {138, 145}, {122, 145}, {106, 145},
};

// TODO: PROGMEM
const bool FONT_3x5[10][5][3] = {
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
display_draw_number(uint8_t number, uint8_t x, uint8_t y, uint16_t fg_color, uint16_t bg_color, uint8_t magnify = 1) {
    for (int xx = 0; xx < 3; xx++) {
        for (int yy = 0; yy < 5; yy++) {
            uint16_t color = FONT_3x5[number][yy][xx] ? fg_color : bg_color;
            int x1 = x + xx * magnify;
            int y1 = y + yy * magnify;
            tft.fillRectangle(x1, y1, x1 + magnify, y1 + magnify, color);
        }
    }
}

void display_draw_labels() {
    tft.setFont(Terminal6x8);

    tft.drawText(35, 42, "VOLTS", COLOR_WHITE);
    tft.drawText(131, 42, "MAH", COLOR_WHITE);

    tft.drawText(90, 125, "KPH", COLOR_WHITE);

    tft.drawText(24, 173, "TRIP", COLOR_WHITE);
    tft.drawText(102, 173, "TOTAL", COLOR_WHITE);
    tft.drawText(69, 200, "KM", COLOR_WHITE);
    tft.drawText(138, 200, "KM", COLOR_WHITE);
}

void fill_zeroes(char *str) {
    for (int i = 0; str[i] == ' '; i++)
        str[i] = '0';
}

void display_set_volts(float volts) {
    char fmt[5];
    dtostrf(volts, 4, 1, fmt);
    fill_zeroes(fmt);
    tft.setFont(Terminal12x16);
    tft.drawText(24, 25, fmt, COLOR_WHITE);
}

void display_set_mah(uint16_t mah) {
    char fmt[6];
    dtostrf(mah, 5, 0, fmt);
    fill_zeroes(fmt);
    tft.setFont(Terminal12x16);
    tft.drawText(88, 25, fmt, COLOR_WHITE);
}

void display_set_trip_distance(uint32_t meters, uint16_t color = COLOR_WHITE) {
    char fmt[7];
    dtostrf(meters / 1000.0, 5, 2, fmt);
    fill_zeroes(fmt);
    tft.setFont(Terminal12x16);
    tft.drawText(24, 183, fmt, color);
}

void display_set_total_distance(uint32_t meters) {
    char fmt[6];
    dtostrf(meters / 1000.0, 4, 0, fmt);
    fill_zeroes(fmt);
    tft.setFont(Terminal12x16);
    tft.drawText(102, 183, fmt, COLOR_WHITE);
}

void display_set_speed(uint8_t kph) {
    if (kph >= 10)
        display_draw_number(kph / 10, 60, 85, COLOR_WHITE, COLOR_BLACK, 7);
    else
        tft.fillRectangle(60, 85, 82, 121, COLOR_BLACK);
    display_draw_number(kph % 10, 89, 85, COLOR_WHITE, COLOR_BLACK, 7);
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
    tft.fillRectangle(85, 149, 89, 153, tft.setColor(0, 150, 0));
    delay(duration_ms);
    tft.fillRectangle(85, 149, 89, 153, COLOR_BLACK);
}

void display_indicate_read_failure(uint32_t duration_ms) {
    tft.fillRectangle(85, 149, 89, 153, tft.setColor(150, 0, 0));
    delay(duration_ms);
    tft.fillRectangle(85, 149, 89, 153, COLOR_BLACK);
}

uint16_t display_make_color(uint8_t red, uint8_t green, uint8_t blue) {
    return tft.setColor(red, green, blue);
}
