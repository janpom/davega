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

#ifndef DAVEGA_DEFAULT_SCREEN_H
#define DAVEGA_DEFAULT_SCREEN_H

#include <TFT_22_ILI9225.h>
#include "davega_ili9225_screen.h"
#include "vesc_comm.h"

class DavegaDefaultScreen: public DavegaILI9225Screen {
public:
    void reset();
    void update(t_davega_data* data);
    void heartbeat(uint32_t duration_ms, bool successful_vesc_read);

protected:
    // Have we just reset the screen? Unset by the first update() call.
    bool _just_reset = false;

    // Remember how many cells are currently filled so that we can update the indicators more efficiently.
    uint8_t _battery_cells_filled = 0;
    uint8_t _speed_cells_filled = 0;

    vesc_comm_fault_code _last_fault_code = FAULT_CODE_NONE;

    void _draw_labels();
    bool _draw_battery_cell(int index, bool filled, bool redraw = false);
    void _draw_speed_cell(int index, bool filled, bool redraw = false);
    void _update_battery_indicator(float battery_percent, bool redraw = false);
    void _update_speed_indicator(float speed_percent, bool redraw = false);
};

#endif //DAVEGA_DEFAULT_SCREEN_H
