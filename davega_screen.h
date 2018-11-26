/*
    Copyright 2018 Jan Pomikalek <jan.pomikalek@gmail.com>

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

#ifndef DAVEGA_SCREEN_H
#define DAVEGA_SCREEN_H

#include <TFT_22_ILI9225.h>
#include "davega_data.h"

typedef struct {
    char* fw_version;
    bool imperial_units;
    bool per_cell_voltage;
    uint8_t battery_cells;
} t_davega_screen_config;

class DavegaScreen {
public:
    void init(TFT_22_ILI9225* tft, t_davega_screen_config* config) { _tft = tft; _config = config; };
    virtual void reset() = 0;
    virtual void update(t_davega_data* data) = 0;
    virtual void heartbeat(uint32_t duration_ms, bool successful_vesc_read) = 0;

protected:
    TFT_22_ILI9225* _tft;
    t_davega_screen_config* _config;
};

#endif //DAVEGA_SCREEN_H
