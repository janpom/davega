/*
    Copyright 2019 Jan Pomikalek <jan.pomikalek@gmail.com>

    This file is part of the DAVEga firmware.

    DAVEga firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DAVEga firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with DAVEga firmware.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "davega_mini_screen.h"

#define OLED_RESET 4
Adafruit_SSD1306 oled(OLED_RESET);
Adafruit_SSD1306* p_oled = nullptr;

void DavegaSSD1306Screen::init(t_davega_screen_config *config) {
    DavegaScreen::init(config);
    if (!p_oled) {
        p_oled = &oled;
        p_oled->begin(SSD1306_SWITCHCAPVCC, 0x3C);
        p_oled->clearDisplay();
    }
    _oled = p_oled;
}
