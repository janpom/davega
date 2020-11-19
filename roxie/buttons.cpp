#include "buttons.h"
#include "data.h"
#include "eeprom_backup.h"

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
unsigned long button1_falling_time;
unsigned long button1_rising_time;
volatile bool button1_rising_flag = false;
volatile bool button2_down = LOW;
volatile bool button3_down = LOW;

bool check_button1(t_data* data){
	if (button1_rising_flag == true){
		Serial.println("Button 1 was pressed for:  " + String(button1_rising_time - button1_falling_time) + " ms");
		if ((button1_rising_time - button1_falling_time) > LONG_PRESS){
			// Reset session and also write this to the EEPROM
			data->session->trip_meters = 0;
			data->session->max_speed_kph = 0;
			data->session->millis_elapsed = 0;
			data->session->millis_riding = 0;
			// session_data.min_voltage = data.voltage;
			eeprom_write_session_data(*data->session);
			Serial.println("Session reset");
			button1_rising_flag = false;
			return true;
		}
		button1_rising_flag = false;
	}
	return false;

/*     // uint32_t button_1_down_elapsed = millis() - button_1_last_up_time;
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
			Serial.println("longer than debounce delay"); */


			// *startup_trip_meters = -(*tachometer);
			// initial_millis_elapsed = -millis();
/* 		} */


}

// ISR routines for button presses
void button1_changed(){
	if(digitalRead(BUTTON_1_PIN) == LOW){
		button1_falling_time = millis();
	}
	else
	{
		button1_rising_flag = true;
		button1_rising_time = millis();
	}	
}

void button2_pressed(){
	Serial.println("Button 2 pressed");
}

void button3_pressed(){
	Serial.println("Button 3 pressed");
}