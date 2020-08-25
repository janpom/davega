#include "buttons.h"
#include "data.h"
#include "roxie_eeprom.h"

/* typedef enum {
  HIGH,
  LOW
} t_button_states; */


#define BUTTON_1_PIN A0
#define BUTTON_2_PIN A1
#define BUTTON_3_PIN A2

/* Button */
#define DEBOUNCE_TIME           5     // *10 ms
#define LONGCLICK_TIME          200   // *10 ms
#define DOUBLECLICK_TIME        20    // *10 ms
#define BTTN_CLEAR              0x00
#define BTTN_CLICK              0x01
#define BTTN_LONGCLICK          0x02
#define BTTN_DBLCLICK           0x04
#define BTTN_CLICK_PROCESSED    0x10
#define BTTN_LC_PROCESSED       0x20
#define BTTN_DC_PROCESSED       0x40
#define BTTN_RELEASED_PROCESSED 0x80

typedef enum {
	ONOFF_CLICK = 1,
	ONOFF_CLICK_LONG_CLICK = 2,
	ONOFF_LONG_CLICK = 4,
	UP_CLICK = 8,
	UP_CLICK_LONG_CLICK = 16,
	UP_LONG_CLICK = 32,
	DOWN_CLICK = 64,
	DOWN_CLICK_LONG_CLICK = 128,
	DOWN_LONG_CLICK = 256,
	UPDOWN_CLICK = 512,
	M_CLICK = 1024, // Some devices (SW102) have this extra button
	M_CLICK_LONG_CLICK = 2048,
	M_LONG_CLICK = 4096
} buttons_events_t;


#define DEBOUNCE_DELAY 25

bool current_state = HIGH; // button not pressed, active-low
bool change_flag = false;
unsigned long last_button_falling_time;
unsigned long last_button_rising_time;


void read_buttons(t_session_data session_data){

    // uint32_t button_1_down_elapsed = millis() - button_1_last_up_time;
    if(change_flag){
		if (last_button_rising_time - last_button_falling_time > DEBOUNCE_DELAY) {
			// D("button 1 pressed");

			// reset session
			session_data.trip_meters = 0;
			session_data.max_speed_kph = 0;
			session_data.millis_elapsed = 0;
			session_data.millis_riding = 0;
			// session_data.min_voltage = data.voltage;
			eeprom_write_session_data(session_data);
			// initial_trip_meters = -tachometer;
			// initial_millis_elapsed = -millis();
		}
    }

    if(button3_down){
        button3_down = LOW;
        // D("Button 3 pressed " + String(millis()-starting_time) + " milliseconds");
        // scr = screens[current_screen_index];
        //scr->nextScreen();
        //scr->reset();
    }

}

// ISR routines for button presses
void button1_falling(){
	current_state = LOW;	
	last_button_falling_time = millis();
}

void button1_rising(){
	last_button_rising_time = millis();
	current_state = HIGH;
	change_flag = true;
}

void button2_pressed(){
    button2_down = HIGH;
}

void button3_pressed(){
    button3_down = HIGH;
}

/* 
	const boolean read_state =!digitalRead(BUTTON_1_PIN); // readstate high when button pressed
	if (read_state != current_state) {
		if (millis() > last_button_change_time + DEBOUNCE_DELAY) {
        	current_state = read_state;
        	change_flag = true;
      	} */