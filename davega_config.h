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

#ifndef DAVEGA_CONFIG_H
#define DAVEGA_CONFIG_H

// To compile for FOCBOX Unity, uncomment the following line.
//#define FOCBOX_UNITY 1

#define VESC_COUNT 2  // number of controllers: 1 = single, 2 = dual (set to 1 for FOCBOX Unity unless you have more than 1)
#define MOTOR_POLE_PAIRS 7
#define WHEEL_DIAMETER_MM 192
#define MOTOR_PULLEY_TEETH 15
#define WHEEL_PULLEY_TEETH 72

// Affects the speed indicator. If MAX_SPEED_KPH is exceeded, no major disaster will happen.
// The speed indicator will merely indicate the current speed as the max speed (all blue rectangles
// filled). It will still show the correct number though.
#define MAX_SPEED_KPH 50

// Set to true to display the distance in miles and the speed in mph.
#define IMPERIAL_UNITS false

// Set to true to display the temperature in Fahrenheit instead of Celsius.
#define USE_FAHRENHEIT false

#define BATTERY_S 12  // number of battery cells
#define BATTERY_MAX_MAH 8000  // battery capacity in mAh
#define BATTERY_USABLE_CAPACITY 0.8  // [0.0, 1.0]

// If SHOW_AVG_CELL_VOLTAGE is true, average cell voltage is displayed instead of the total battery
// pack voltage (total voltage is divided by the number of cells).
#define SHOW_AVG_CELL_VOLTAGE true

// The two options below are for detecting that battery has been fully charged (in which case
// we reset the available mAh to the max value). We think that the battery has been fully charged
// if the voltage has increased at least a little bit since the last remembered state AND the
// battery voltage is close to the max value (which is the last value in DISCHARGE_TICKS, see below).

// How much the battery voltage must increase since the last remembered state so that
// so that we think the battery has been charged.
#define FULL_CHARGE_MIN_INCREASE 0.01  // [0.0, 1.0]

// Minimal percentage of the max battery voltage to consider the battery fully charged.
// Example: With 0.97, if the max voltage is 50.4 V, then 48.888 V or more is considered fully charged.
#define FULL_CHARGE_THRESHOLD 0.97  // [0.0, 1.0]

// The DISCHARGE_TICKS can be used for configuring the battery discharge profile. The first value in the
// array is the voltage for fully discharged and the last value is the voltage for fully charged. The
// array may contain any number of values representing equidistant points of the discharge curve. For
// example, with 5 values, the voltages will be interpreted as 0%, 25%, 50%, 75%, and 100% of the battery
// capacity respectively.
#define DISCHARGE_TICKS { \
    3.5 * BATTERY_S, \
    3.67 * BATTERY_S, \
    3.73 * BATTERY_S, \
    3.76 * BATTERY_S, \
    3.80 * BATTERY_S, \
    3.83 * BATTERY_S, \
    3.86 * BATTERY_S, \
    3.90 * BATTERY_S, \
    4.0 * BATTERY_S, \
    4.2 * BATTERY_S, \
}

// Remaining battery capacity can be estimated from the battery voltage
// and from the VESC coulomb counter (number of mAh spent). The VOLTAGE_WEIGHT is
// the weight of the battery voltage in the calculation. (1.0 - VOLTAGE_WEIGHT) is
// then the weight of the coulomb counter.
//
// In particular,
// 1.0 = only use battery voltage for estimating remaining capacity.
// 0.0 = only use coulomb counter for estimating remaining capacity.
#define VOLTAGE_WEIGHT 0.4

// Changing the EEPROM_MAGIC_VALUE (to any value different from the current, e.g. 42 -> 43) will reset
// the EEPROM to the values defined below. This is especially handy for pre-setting the total distance
// traveled (EEPROM_INIT_VALUE_TOTAL_DISTANCE).
#define EEPROM_MAGIC_VALUE 43  // [1, 255]

#define EEPROM_INIT_VALUE_VOLTS 0
#define EEPROM_INIT_VALUE_MAH_SPENT 0
#define EEPROM_INIT_VALUE_MAX_SPEED 0
#define EEPROM_INIT_VALUE_MILLIS_ELAPSED 0
#define EEPROM_INIT_VALUE_MILLIS_RIDING 0
#define EEPROM_INIT_VALUE_MIN_VOLTAGE 60
#define EEPROM_INIT_VALUE_TRIP_DISTANCE 0  // meters
#define EEPROM_INIT_VALUE_TOTAL_DISTANCE 0  // meters

// After how many meters traveled should the distance (and other values) be stored to EEPROM.
// The EEPROM lasts for 100,000 write cycles. With EEPROM_UPDATE_EACH_METERS=100, the EEPROM
// should last for 10,000 km. If the value is set lower, the EEPROM will die earlier.
// Note that EEPROM is also updated whenever the board comes to a stop (see below), so regardless
// of how EEPROM_UPDATE_EACH_METERS is set, there won't be missed meters unless DAVEga is accidentally
// reset before saving to EEPROM (which shouldn't happen under normal circumstances).
#define EEPROM_UPDATE_EACH_METERS 100

// If the board comes to stop, update EEPROM, unless it was already updated in less than
// EEPROM_UPDATE_MIN_DELAY_ON_STOP millis when the board stopped. This shouldn't happen
// under normal circumstance and is mainly a safeguard against destroying EEPROM in case
// unexpected oscillating speed readings are retrieved from the VESC.
//
// The EEPROM will still be updated if it was previously updated in less than
// EEPROM_UPDATE_MIN_DELAY_ON_STOP due to EEPROM_UPDATE_EACH_METERS. (Otherwise, meters would be missed.)
#define EEPROM_UPDATE_MIN_DELAY_ON_STOP 10000

// Hold button 1 for this time to reset session data.
// Hold button 2 for this time to reset the Coulomb counter.
#define COUNTER_RESET_TIME 3000  // ms

// This corresponds (more or less) to how often data is read from VESC.
#define UPDATE_DELAY 50  // ms

// 0=portrait, 1=right rotated landscape, 2=reverse portrait, 3=left rotated landscape
#define SCREEN_ORIENTATION 0

// Screens. Uncomment the ones you want enabled.
//#define DEFAULT_SCREEN_ENABLED 1
//#define SIMPLE_HORIZONTAL_SCREEN_ENABLED 1
//#define SIMPLE_HORIZONTAL_SCREEN_WITH_BATTERY_CURRENT_ENABLED 1
//#define SIMPLE_HORIZONTAL_SCREEN_WITH_MOTOR_CURRENT_ENABLED 1
#define SIMPLE_VERTICAL_SCREEN_ENABLED 1
//#define SIMPLE_VERTICAL_SCREEN_WITH_BATTERY_CURRENT_ENABLED 1
//#define SIMPLE_VERTICAL_SCREEN_WITH_MOTOR_CURRENT_ENABLED 1
#define TEXT_SCREEN_ENABLED 1

// Information to be displayed on the text screen. Only relevant if TEXT_SCREEN_ENABLED is set.
// Available options:
//    SCR_FW_VERSION
//    SCR_FAULT_CODE
//    SCR_MOSFET_TEMPERATURE
//    SCR_MOTOR_TEMPERATURE
//    SCR_MOTOR_CURRENT
//    SCR_BATTERY_CURRENT
//    SCR_DUTY_CYCLE
//    SCR_TOTAL_VOLTAGE
//    SCR_MIN_TOTAL_VOLTAGE
//    SCR_AVG_CELL_VOLTAGE
//    SCR_BATTERY_CAPACITY_MAH
//    SCR_BATTERY_CAPACITY_PERCENT
//    SCR_TRIP_DISTANCE
//    SCR_TOTAL_DISTANCE
//    SCR_SPEED
//    SCR_MAX_SPEED
//    SCR_AVG_SPEED
//    SCR_TIME_ELAPSED
//    SCR_TIME_RIDING
#define TEXT_SCREEN_ITEMS { \
    SCR_FAULT_CODE, \
    SCR_MOSFET_TEMPERATURE, \
    SCR_MOTOR_TEMPERATURE, \
    SCR_MOTOR_CURRENT, \
    SCR_BATTERY_CURRENT, \
    SCR_DUTY_CYCLE, \
    SCR_TOTAL_VOLTAGE, \
    SCR_MIN_TOTAL_VOLTAGE, \
    SCR_AVG_CELL_VOLTAGE, \
    SCR_BATTERY_CAPACITY_MAH, \
    SCR_BATTERY_CAPACITY_PERCENT, \
    SCR_TRIP_DISTANCE, \
    SCR_TOTAL_DISTANCE, \
    SCR_SPEED, \
    SCR_MAX_SPEED, \
    SCR_AVG_SPEED, \
    SCR_TIME_ELAPSED, \
    SCR_TIME_RIDING, \
}

// Big font is only recommended for landscape orientation. 9 lines fit on the screen then.
// Some items overflow the right order. You may want to adjust the labels in davega_text_screen.cpp.
#define TEXT_SCREEN_BIG_FONT false

#endif //DAVEGA_DAVEGA_CONFIG_H
