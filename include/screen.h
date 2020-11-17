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

#ifndef SCREEN_H
#define SCREEN_H

#define HEARTBEAT_SIZE 4

#include "data.h"

class Screen {
public:
    virtual void init(t_screen_config* config) { _config = config; };
    virtual void draw_basic() = 0;
    virtual void update(t_data* data) = 0;
    virtual void heartbeat(uint32_t duration_ms, bool successful_vesc_read) = 0;

protected:
    t_screen_config* _config;
};

#endif //SCREEN_H