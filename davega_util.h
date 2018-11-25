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

#ifndef DAVEGA_UTIL_H
#define DAVEGA_UTIL_H

#include "vesc_comm.h"
#include <TFT_22_ILI9225.h>

#define KM_PER_MILE 0.621371

char* make_fw_version(char* fw_version, char* revision_id);
float convert_distance(float distance_km, bool imperial_units);
float convert_speed(float speed_kph, bool imperial_units);
char* vesc_fault_code_to_string(vesc_comm_fault_code fault_code);
uint16_t progress_to_color(float progress, TFT_22_ILI9225* tft);

#endif //DAVEGA_UTIL_H
