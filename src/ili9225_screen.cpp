/*
    This file is part of the Roxie firmware.
    
    Roxie firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Roxie firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Roxie firmware.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "ili9225_screen.h"
#include "roxie_config.h"

#define TFT_RS  9   // REGISTER SELECT
#ifdef ARDUINO_NANO_EVERY
    #define TFT_CS  8  // CS
    #define TFT_RST 10  // TODO: 12 does not work, check why
#else 
    #define TFT_CS  10  // CS
    #define TFT_RST 12
#endif
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 0

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, 10); // hardware SPI
// TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_SDI, TFT_CLK, TFT_LED, 200); // software SPI
TFT_22_ILI9225* p_tft = nullptr;

void ILI9225Screen::init(t_screen_config *config) {
    Screen::init(config);
    if (!p_tft) {
        p_tft = &tft;
        p_tft->begin();
        p_tft->setOrientation(config->orientation);
        p_tft->setBackgroundColor(COLOR_BLACK);
    }
    _tft = p_tft;
}