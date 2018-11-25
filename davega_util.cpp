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

#include "davega_util.h"
#include "vesc_comm.h"
#include <TFT_22_ILI9225.h>

char fw_version_buffer[6];

char* make_fw_version(char* fw_version, char* revision_id) {
    if (fw_version[0] == 'v') {
        return fw_version;
    }
    else {
        String r = String("r");
        String upper_rev_id = String(revision_id).substring(5, 9);
        upper_rev_id.toUpperCase();
        r.concat(upper_rev_id);
        r.toCharArray(fw_version_buffer, sizeof(fw_version_buffer));
        return fw_version_buffer;
    }
}

float convert_distance(float distance_km, bool imperial_units)
{
    if (imperial_units)
        return distance_km * KM_PER_MILE;
    else
        return distance_km;
}

float convert_speed(float speed_kph, bool imperial_units)
{
    return convert_distance(speed_kph, imperial_units);
}

char* vesc_fault_code_to_string(vesc_comm_fault_code fault_code) {
    switch (fault_code) {
        case FAULT_CODE_NONE:
            return "NONE";
        case FAULT_CODE_OVER_VOLTAGE:
            return "OVER VOLTAGE";
        case FAULT_CODE_UNDER_VOLTAGE:
            return "UNDER VOLTAGE";
        case FAULT_CODE_DRV:
            return "DRV FAULT";
        case FAULT_CODE_ABS_OVER_CURRENT:
            return "OVER CURRENT";
        case FAULT_CODE_OVER_TEMP_FET:
            return "OVER TEMP FET";
        case FAULT_CODE_OVER_TEMP_MOTOR:
            return "OVER TEMP MOTOR";
        default:
            return "unexpected fault code";
    }
}

uint16_t progress_to_color(float progress, TFT_22_ILI9225* tft) {
    float brightness = 255.0 * (1.0 - progress);
    return  tft->setColor(brightness, brightness, brightness);
}
