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

#ifndef BUTTONS_H
#define BUTTONS_H

#include "data.h"


volatile bool button1_clicked = false;
volatile bool button2_down = LOW;
volatile bool button3_down = LOW;

void read_buttons(t_session_data session_data);
void button1_falling();
void button1_rising();
void button2_pressed();
void button3_pressed();


#endif //BUTTONS_H