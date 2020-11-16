#include "buttons.h"
#include "data.h"
#include "roxie_eeprom.h"

/* typedef enum {
  HIGH,
  LOW
} t_button_states; */

#ifdef ARDUINO_NANO_EVERY
	#define BUTTON_1_PIN A0
	#define BUTTON_2_PIN A1
	#define BUTTON_3_PIN A2
#else
	#define BUTTON_1_PIN PB3
	#define BUTTON_2_PIN PB4
	#define BUTTON_3_PIN PB5
#endif

#define DEBOUNCE_DELAY 25
#define LONG_PRESS 1000

bool current_state = HIGH; // button not pressed, active-low
bool change_flag = false;
unsigned long last_button_falling_time;
unsigned long last_button_rising_time;
volatile bool button2_down = LOW;
volatile bool button3_down = LOW;

void read_buttons(t_session_data &session_data, int32_t* initial_trip_meters, int32_t* tachometer, Screen* scr){

    // uint32_t button_1_down_elapsed = millis() - button_1_last_up_time;
    if(change_flag){
		Serial.println("change flag was true");
		Serial.println("down time was  " + String(last_button_rising_time - last_button_falling_time));
		change_flag = false;
		if ((last_button_rising_time - last_button_falling_time) > LONG_PRESS)
		{
			Serial.println("long press");
		}
		else if ((last_button_rising_time - last_button_falling_time) > DEBOUNCE_DELAY)
		{
			Serial.println("longer than debounce delay");

			// reset session
			session_data.trip_meters = 0;
			session_data.max_speed_kph = 0;
			session_data.millis_elapsed = 0;
			session_data.millis_riding = 0;
			// session_data.min_voltage = data.voltage;
			eeprom_write_session_data(session_data);
			*initial_trip_meters = -(*tachometer);
			// initial_millis_elapsed = -millis();
		}
    }

    if(button3_down){
        button3_down = LOW;
        // scr = screens[current_screen_index];
        scr->nextScreen();
        scr->reset();
    }

}

// ISR routines for button presses
void button1_changed(){
	const bool read_state = digitalRead(BUTTON_1_PIN); // read_state low when button pressed
	if(!read_state){
		last_button_falling_time = millis();
	}
	else{
		last_button_rising_time = millis();
		change_flag = true;
	}
}

void button2_pressed(){
    button2_down = HIGH;
}

void button3_pressed(){
    button3_down = HIGH;
}