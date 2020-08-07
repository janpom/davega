/*
    Copyright 2019 Jan Pomikalek <jan.pomikalek@gmail.com>

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

#ifndef ILI9225_SCREEN_H
#define ILI9225_SCREEN_H

#include "screen.h"
#include <TFT_22_ILI9225.h>

class ILI9225Screen: public Screen {
public:
    void init(t_screen_config* config) override;

protected:
    TFT_22_ILI9225* _tft;
};

#endif //ILI9225_H
