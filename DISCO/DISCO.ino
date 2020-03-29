/*
    DISCO - a arduino programm with music and leds for NWT
    Copyright (C) 2020  Johannes Schmatz, Alwin Fronius

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
////// README:
////// DOKU am Ende der Datei
////// Quellen in der DOKU
////// Bitte konfigurieren! (in '<config>[...]</config>')
// <config>
	//// pins definieren  
	// pin fuer lautsprecher
	int tonepin = 3;
  
	// pin fuer servo (gelb am servo)
	int servopin = 4;

	// pin fuer den taster
	int tasterpin = 5;

	// pin fuer die lichtschranke
	int lichtschrankenpin = 6;

	// pin fuer den schalter, 
	int refrain_schalter = 7;

	// pin fuer die status-LED
	int onboardled = 13;

	//// die Spiel-Geschwindigkeit, Schlaege pro Minute und die Laenge fuer den Neuanschlag
	int speed = 1;
	int schlaege_pro_min = 72;
	int t_neuanschlag = 20;

	//// pins fuer den Schieberegister
	int pin_74HC595_data = 8;
	int pin_74HC595_stcp = 9;
	int pin_74HC595_clock = 10;
	int pin_74HC595_reset = 11;

	///// sonder funktionen
	/// Serielle Steuerung
	#define SERIAL
	
	/// toene eine oktave tiefer 
	//#define TOENE_TIEFER

	/// servo
	#define SERVO
	int servo_null = 75;

	///// refrain spielen <ja/nein/schalter waehlen>
	/// nur einen waehlen
	/// ja 
	//#define PLAY_REFRAIN
	/// nein
	//#define PLAY_REFRAIN_NOT
	/// schalter
	#define PLAY_REFRAIN_SWICH
// </config>

// special functions for testing etc. MAY NOT WORKING
//#define HARDCODED_LENGHT
// end specials

// includes etc.
#include <Arduino.h> // needet by arduino

#ifdef SERIAL // declaration of the variable, which contains the serial input (only needet when serial is needet)
int serial_in;
#endif

#ifdef SERVO // definiton of the servo and the include of the library for the servo (only needet when servo is needet)
#include <Servo.h> 
Servo myservo; // erstellt ein Servo-Objekt um einen Servomotor zu steuern
#endif

//// declaration and definiton of the variables
boolean noleds = false; // the noleds variable
boolean inled = false; // var contains if we are in leds()
int servo_hidden = 180 - servo_null; // hidden position for the person
int servo_pos; // variable that contains the position of the servo
int sr_high = 1; // the HIGH state for the shift register
int sr_low = 0; // the LOW state for the shift register
int state[2]; // vars that contains state of button


// variables for the output of the shift register
int sr_a = sr_low; // pin QA
int sr_b = sr_low; // pin QB
int sr_c = sr_low;
int sr_d = sr_low; // ...
int sr_e = sr_low;
int sr_f = sr_low;
int sr_g = sr_low; // pin QG
int sr_h = sr_low; // pin QH

// variables for the tone lenght
int t_ganze;
int t_halbe;
int t_viertel;
int t_viertel_t; // viertel triole
int t_achtel;
int t_achtel_t; // achtel triolen
int t_sechzehntel;
int t_sechzehntel_t; // sechzehntel triolen
int t_zweisechzehntel;

//// all functions
void setup(){
	// pinMode for all pins
	pinMode(tasterpin, INPUT_PULLUP);
	pinMode(lichtschrankenpin, INPUT_PULLUP);
	pinMode(refrain_schalter, INPUT_PULLUP);
	pinMode(onboardled, OUTPUT);

	digitalWrite(onboardled, HIGH); // schalte onboardled an
	init_74HC595(); // initalisiert den Schieberegister
	
	for(int i=0; i<2; i++){ // init state var
		state[i] = 0;
	}

#ifdef SERVO // only when servo is needet
	myservo.attach(servopin); // verknüpft den Servomotor an servopin mit dem Servo-Objekt
#endif

#ifdef SERIAL // only when Seral is needet
	Serial.begin(9600); // serial communication
	while(!Serial){;} // serial connect loop
	Serial.println("Hello_World!_0_=_song();_1_=_leds();_2_=_noleds_song();_3_voltmeter();."); // only one word -> only one string in C (for future)
#endif
	 servo(servo_hidden); // hide person
	set_tones(); // tone lenght set
	noleds = false; // false = bei song blinken LEDs
	digitalWrite(onboardled, LOW); // schalte onboardled aus
	delay(100);
	timesignal(4);
	servo(0);
	led_blink();
}
void loop(){
	if(digitalRead(tasterpin) == LOW){ // wenn taster gedrueckt
		song();
	}
	if(digitalRead(lichtschrankenpin) == HIGH){ // lichtsensor
		leds();
	}
#ifdef SERIAL // serial menu
	serial_in = Serial.read();
	if(serial_in == '0'){ // seriell music + leds
		Serial.println("song();");
		song();
	}
	if(serial_in == '1'){ // seriell !music + leds
		Serial.println("leds();");
		leds();
	}
	if(serial_in == '2'){ // seriell music + !leds
		Serial.println("noleds_song();");
		noleds_song();
	}
	if(serial_in == '3'){ // serielles Voltmeter
		Serial.println("voltmeter();");
		voltmeter();
	}
}
void noleds_song(){ // function for calling the song without leds
	noleds = true; // true = keine LEDs
	song();
}
void voltmeter(){ // function for a voltmeter (not conected)
	while(1){ // you have to reset your arduino after using the voltmeter
		Serial.print(map(analogRead(A0), 0, 1023, 0, 5000)); // print on serial the value form 0 to 1024 mapped to 0 to 5000
		Serial.println("mV"); // print mV
		delay(100); // wait 100ms
}
#endif
}
int refrain_play(){ // Funktion, die zurueckgibt, ob der Refrain (teils zweite Stimme) gespielt werden soll
	int ref_play =  digitalRead(refrain_schalter);
#ifdef PLAY_REFRAIN
	return 1; // play the refrain
#endif
#ifdef PLAY_REFRAIN_NOT
	return 0; // maybe DEAD CODE // play the refrain not
#endif
	return ref_play; // maybe DEAD CODE
}
void timesignal(int number_of_beeps){
	// BBC time signal (as init beep)
	for(int i=0; i!=number_of_beeps; i++){
		digitalWrite(onboardled, HIGH);
		tone(tonepin, 1000);
		delay(100);
		digitalWrite(onboardled, LOW);
		noTone(tonepin); 
		delay(900);
	}
	digitalWrite(onboardled, HIGH);
	tone(tonepin, 1000);
	delay(1000);
	digitalWrite(onboardled, LOW);
	noTone(tonepin);
}
void set_tones(){ // function that sets the lenght of tones
#ifdef HARDCODED_LENGHT // lenght of tones hardcodet
	t_ganze = 3284;
	t_halbe = 1642;
	t_viertel = 821;
	t_viertel_t = 547;
	t_achtel = 410;
	t_achtel_t = 273:
	t_sechzehntel = 205;
	t_sechzehntel_t = 136;
	t_zweisechzehntel = 102;
#endif
#ifndef HARDCODED_LENGHT // calculate the lenght of tones
	t_viertel = 60000 / ( schlaege_pro_min * speed );
	t_ganze = t_viertel * 4;
	t_halbe = t_viertel * 2;
	t_viertel_t = t_halbe / 3;
	t_achtel = t_viertel / 2;
	t_achtel_t =t_viertel / 3;
	t_sechzehntel = t_viertel / 4;
	t_sechzehntel_t = t_viertel / 6;
	t_zweisechzehntel = t_viertel / 8;
#endif
}
void servo(int pos){ // Servo write with servo(pos); (pos is relative to 90° or up), but only if servo is needet
	if(pos < -35){
		return; // return for the person
	}
	pos = servo_null + pos; // make pos relative to servo_null
	servo_pos = pos; // save the position
	servo_write(pos);
}
void servo_rel(int pos){ // Servo write relative to last position
	pos = servo_pos + pos; // make pos relative to servo_pos
	servo_pos = pos; // save the position
	servo_write(pos);
}
void servo_toggle(int minpos, int maxpos){ // move servo to other position 
	maxpos = servo_null + maxpos; // make maxpos relative to servo_null
	minpos = servo_null + minpos; // make minpos relative to servo_null
	if(servo_pos == maxpos){ // if on maxpos
		servo_pos = minpos; // save pos
		servo_write(minpos);  // goto minpos
	}else{ // else
		servo_pos = maxpos; // save pos
		servo_write(maxpos); // goto maxpos
	}
}
void servo_write(int pos){ // direct servo write with protection
#ifdef SERVO
	if(pos > 180 || pos < 0 || pos < 40){
		return; // return because then the servo can't go in that position
	}
	myservo.write(pos);
#endif
}
void sr_dw(int state){ // function to write 0 or 1 to the pin DATA of the shift register
	if(state==sr_high){
		digitalWrite(pin_74HC595_data, HIGH);
	}else{
		digitalWrite(pin_74HC595_data, LOW);
	}
}
void write_74HC595(int pos, int state){ // args 0 or 1, 0-7 for pins of Shift Register (sr)
	// writing in the state vaiables the actual state
	if(pos==0){sr_a = state;}
	if(pos==1){sr_b = state;}
	if(pos==2){sr_c = state;}
	if(pos==3){sr_d = state;}
	if(pos==4){sr_e = state;}
	if(pos==5){sr_f = state;}
	if(pos==6){sr_g = state;}
	if(pos==7){sr_h = state;}

	// writing the states out
	sr_dw(sr_h);
	shift_74HC595();
	sr_dw(sr_g);
	shift_74HC595();
	sr_dw(sr_f);
	shift_74HC595();
	sr_dw(sr_e);
	shift_74HC595();
	sr_dw(sr_d);
	shift_74HC595();
	sr_dw(sr_c);
	shift_74HC595();
	sr_dw(sr_b);
	shift_74HC595();	
	sr_dw(sr_a);
	shift_74HC595();
	show_74HC595();
}
void show_74HC595(){ // function to give a short HIGH on pin_74HC595_stcp -> refreshes the latch
	digitalWrite(pin_74HC595_stcp, HIGH);
	wait_some_time();
	digitalWrite(pin_74HC595_stcp, LOW);
	wait_some_time();
}
void shift_74HC595(){ // function to shift (short HIGH) on pin_74HC595_clock -> shift data in
	digitalWrite(pin_74HC595_clock, HIGH);
	wait_some_time();
	digitalWrite(pin_74HC595_clock, LOW);
	wait_some_time();
}
void init_74HC595(){ // function to initalize the shift register
	init_74HC595_pm(); // set the pinModes
	init_74HC595_dw(); // set all pins on the right states
}
void init_74HC595_pm(){ // function to set the pinModes for the shift register
	pinMode(pin_74HC595_data, OUTPUT);
	pinMode(pin_74HC595_stcp, OUTPUT);
	pinMode(pin_74HC595_clock, OUTPUT);
	pinMode(pin_74HC595_reset, OUTPUT);
}
void init_74HC595_dw(){ // init the states (LOW or HIGH) on the pins (only seperated for reset use in the future)
	digitalWrite(pin_74HC595_reset, LOW);
	digitalWrite(pin_74HC595_data, LOW);
	digitalWrite(pin_74HC595_clock, LOW);
	digitalWrite(pin_74HC595_stcp, LOW);
	wait_some_time();
	digitalWrite(pin_74HC595_reset, HIGH);
	show_74HC595();
}
	// only to wait some time, for the shift register, and for calling eegg();
	// block formatted, because the function calculates something, but throws the result away.
	// function that waits some time (not with delay(), because it's less time)
	// 9 lines * 22 chars = 192 chars C-code 
void wait_some_time(){
int a=42,b=23,c=5,x,y;
for(int i=23;i!=42;i++
){y=a*(i-b)*(i-b)+c;a=
0;if(y==a){y=(-1)*a*(i
-b)*(i+c);a=y;}else{y=
a*i*i+b*i+c;b=y;}x=a+b
-y-i;c++;}a=42*x-23*c+
b*42-y*5+23+y;eegg();}
	/* Description for the function wait_some_time(); (all code in the description is better formatted than in reality, but it's the same code)
	 *
	 * declare a function that can return nothing (-> resut is throwed away)
	 *	void wait_some_time(){
	 *
	 * first declare some integers (in function short formatted)
	 *		 int a = 42;
	 *		 int b = 23;
	 *		 int c = 5;
	 *		 int x;
	 *		 int y;
	 *		 for(int i = 23; i != 42; i = i + 1){
	 *			 y = a * ( i - b ) * ( i - b ) + c;
	 *			 a = 0;
	 *			 if(y == a){
	 *				 y = - (a * (i - b) * (i + c));
	 *				 a = y;
	 *			 }else{
	 *				 y = i * (a * i + b) + c;
	 *				 b = y;
	 *			 }
	 *			 x = a + b - y - i;
	 *			 c = c + 1;
	 *		 }
	 *
	 * calculate something (better formatted)
	 *		 a = 42 * (x + b) - 23 * c - y * 6 + 23;
	 * call eegg();
	 *	       eegg();
	 *
	 * end of the function (retuns nothing)
	 * 
	 *	 }
	 *
	 * end values:
	 *	 a = 11603
	 *	 b = -20331
	 *	 c = 24
	 *	 x = -41
	 *	 y = -20331
	*/

	// Easter-Egg func.
	// block formatted -> it's a Easter-Egg (nobody can read code)
	// 4 lines * 36 chars = 144 chars C-code
void eegg(){if( inleds != false ){ if
( digitalRead(tasterpin) == LOW ){ if
(play_refrain()==0){final_countdown()
;}else{drunken_sailor();}}}}/////////
	/* desciption for eegg()
		void eegg(){
	if inleds != false -> if we are in leds
			if(inleds != false){
	if button was pressed
				if(digitalRead(tasterpin) == LOW){
	if swich is in pos. 1 play final countdown else drunken sailor
					if(play_refrain() == 0){
						final_countdown();
					}else{
						drunken_sailor();
					}
				}
			}
		}
	end of function
*/
	
void song(){ // music + leds + servo
	digitalWrite(onboardled, HIGH); // turn on the led while playing
	servo(0); // set servo to 0
	/*
	HELP for reading in bohemain_rhapsody(){...}:
		led[0-7]([0-1]);------------------ turns on or off the leds
		   |---| \---\-------------------- status of the led
		   \---\-------------------------- led number
		servo(0);------------------------- set the servo to the position
		      \--------------------------- position of the servo
		[a-b][c-h]([t_achtel_t-t_ganze]);- make tones
		|---||---| \------------------\--- length of the tone
		|---|\---\------------------------ name of the tone
		\---\----------------------------- okave of the tone
		d([t_neuanschlag-t_ganze]);------- wait some time
		  \---------------------\--------- time to wait
	*/
	bohemian_rhapsody(); // start song
	all_leds(0); // turn all leds off
	nt(); // turn ton off	
	digitalWrite(onboardled, LOW); // turn the onboardled off
#ifdef SERIAL // menu only needet when serial is needet
	Serial.println("main();");
#endif
}

// led on off functions (maybe add more)
void led0(int state){if(noleds == false){write_74HC595(0, state);};}
void led1(int state){if(noleds == false){write_74HC595(1, state);};}
void led2(int state){if(noleds == false){write_74HC595(2, state);};}
void led3(int state){if(noleds == false){write_74HC595(3, state);};}
void led4(int state){if(noleds == false){write_74HC595(4, state);};}
void led5(int state){if(noleds == false){write_74HC595(5, state);};}
void led6(int state){if(noleds == false){write_74HC595(6, state);};}
void led7(int state){if(noleds == false){write_74HC595(7, state);};}
void all_leds(int state){ // function to set all leds
	led0(state);
	led1(state);
	led2(state);
	led3(state);
	led4(state);
	led5(state);
	led6(state);
	led7(state);
}
void led_blink(){ // blinking leds (for init)
	led0(1);
	d(100);
	led1(1);
	d(100);
	led2(1);
	d(100);
	led3(1);
	d(100);
	led4(1);
	d(100);
	led5(1);
	d(100);
	led6(1);
	d(100);
	led7(1);
	d(500);
	all_leds(0);
	d(500);
	all_leds(1);
	d(250);
	all_leds(0);
}
void d(int delaytime){(// delay
	nt();
	delay(delaytime);
}
void nt(){// == noTone(tonepin);
	noTone(tonepin);
}
void hold(){ // function to stop and start playing
	state[1] = 0;
	state[0] = 0;
	int laststate = state[1];
	do{
		digitalWrite(onboardled, !digitalRead(onboardled));
		laststate = state[1];
		state[1] = state[0];
		state[0] = digitalRead(tasterpin);
		delay(20);
	}while(!(laststate == 0 && state[1] == 1));

}
void delay_own(int deltime){ // own delay function with stop button
	int laststate = state[1];
	state[1] = state[0];
	state[0] = digitalRead(tasterpin);
	if(laststate == 0 && state[1] == 1){
		hold();
	}
	delay(deltime);
}
// tones for the spekaer
#ifdef TOENE_TIEFER
void ha   (int waittime){tone(tonepin,  220); delay_own(waittime);}
void hh   (int waittime){tone(tonepin,  233); delay_own(waittime);}
void ac   (int waittime){tone(tonepin,  261); delay_own(waittime);}
void acis (int waittime){tone(tonepin,  277); delay_own(waittime);}
void ad   (int waittime){tone(tonepin,  293); delay_own(waittime);}
void adis (int waittime){tone(tonepin,  311); delay_own(waittime);}
void ae   (int waittime){tone(tonepin,  329); delay_own(waittime);}
void af   (int waittime){tone(tonepin,  349); delay_own(waittime);}
void afis (int waittime){tone(tonepin,  369); delay_own(waittime);}
void ag   (int waittime){tone(tonepin,  391); delay_own(waittime);}
void agis (int waittime){tone(tonepin,  415); delay_own(waittime);}
void aa   (int waittime){tone(tonepin,  440); delay_own(waittime);}
void aais (int waittime){tone(tonepin,  466); delay_own(waittime);}
void ah   (int waittime){tone(tonepin,  493); delay_own(waittime);}
void bc   (int waittime){tone(tonepin,  523); delay_own(waittime);}
void bcis (int waittime){tone(tonepin,  554); delay_own(waittime);}
void bd   (int waittime){tone(tonepin,  587); delay_own(waittime);}
void bdis (int waittime){tone(tonepin,  622); delay_own(waittime);}
void be   (int waittime){tone(tonepin,  659); delay_own(waittime);}
void bf   (int waittime){tone(tonepin,  698); delay_own(waittime);}
void bfis (int waittime){tone(tonepin,  739); delay_own(waittime);}
void bg   (int waittime){tone(tonepin,  783); delay_own(waittime);}
void bgis (int waittime){tone(tonepin,  830); delay_own(waittime);}
void ba   (int waittime){tone(tonepin,  880); delay_own(waittime);}
void bais (int waittime){tone(tonepin,  923); delay_own(waittime);}
void bh   (int waittime){tone(tonepin,  987); delay_own(waittime);}
#endif
#ifndef TOENE_TIEFER
void ha   (int waittime){tone(tonepin,  440); delay_own(waittime);}
void hh   (int waittime){tone(tonepin,  466); delay_own(waittime);}
void ac   (int waittime){tone(tonepin,  523); delay_own(waittime);}
void acis (int waittime){tone(tonepin,  554); delay_own(waittime);}
void ad   (int waittime){tone(tonepin,  587); delay_own(waittime);}
void adis (int waittime){tone(tonepin,  622); delay_own(waittime);}
void ae   (int waittime){tone(tonepin,  659); delay_own(waittime);}
void af   (int waittime){tone(tonepin,  698); delay_own(waittime);}
void afis (int waittime){tone(tonepin,  739); delay_own(waittime);}
void ag   (int waittime){tone(tonepin,  783); delay_own(waittime);}
void agis (int waittime){tone(tonepin,  830); delay_own(waittime);}
void aa   (int waittime){tone(tonepin,  880); delay_own(waittime);}
void aais (int waittime){tone(tonepin,  932); delay_own(waittime);}
void ah   (int waittime){tone(tonepin,  987); delay_own(waittime);}
void bc   (int waittime){tone(tonepin, 1046); delay_own(waittime);}
void bcis (int waittime){tone(tonepin, 1108); delay_own(waittime);}
void bd   (int waittime){tone(tonepin, 1174); delay_own(waittime);}
void bdis (int waittime){tone(tonepin, 1244); delay_own(waittime);}
void be   (int waittime){tone(tonepin, 1318); delay_own(waittime);}
void bf   (int waittime){tone(tonepin, 1396); delay_own(waittime);}
void bfis (int waittime){tone(tonepin, 1479); delay_own(waittime);}
void bg   (int waittime){tone(tonepin, 1567); delay_own(waittime);}
void bgis (int waittime){tone(tonepin, 1661); delay_own(waittime);}
void ba   (int waittime){tone(tonepin, 1760); delay_own(waittime);}
void bais (int waittime){tone(tonepin, 1864); delay_own(waittime);}
void bh   (int waittime){tone(tonepin, 1975); delay_own(waittime);}
#endif

// all longer functions
void leds(){ // function for leds
	inled = true; // we are in leds
	// LED blink with shift register
	for(int i=0; i<4; i++){ // 4 times
		servo_toggle(-23, 23);
		led7(1);
		led6(1);
		led3(1);
		led0(1);
		led1(1);
		delay(50);
		led0(0);
		delay(50);
		led4(1);
		led7(0);
		delay(50);
		led5(1);
		led1(0);
		delay(50);
		led7(1);
		led2(1);
		led4(0);
		led6(0);
		delay(50);
		led3(0);
		led7(0);
		led5(0);
		led2(0);
		led0(1);
		delay(50);
		led1(1);
		delay(50);
		led2(1);
		led0(0);
		delay(100);
		led3(1);
		led7(1);
		delay(50);
		led6(1);
		led4(1);
		led1(0);
		led2(0);
		delay(150);
		led0(1);
		led1(1);
		led2(1);
		led3(0);
		led4(0);
		led5(1);
		led6(0);
		led7(0);
		delay(25);
		led0(0);
		led1(1);
		led2(0);
		led7(1);
		delay(25);
		servo_toggle(-23, 23);
		led1(0);
		led2(1);
		led4(1);
		led5(0);
		led6(1);
		led7(0);
		delay(200);
		led1(1);
		led5(1);
		delay(100);
		led0(1);
		led1(0);
		led4(0);
		led6(0);
		led7(1);
		delay(50);
		led5(0);
		led4(1);
		led7(1);
		delay(50);
		led6(1);
		led7(0);
		delay(50);
		led0(0);
		delay(50);
		led1(1);
		delay(50);
		led1(0);
		led0(1);
		led3(1);
		led4(0);
		delay(50);
		led0(0);
		led6(0);
		led5(1);
		delay(50);
		led5(0);
		led4(1);
		led6(1);
		led3(0);
		delay(50);
		led4(0);
		led6(0);
		led5(1);
		delay(50);
		led5(0);
		delay(50);
		led2(0);
	}
	servo(-35);
	inled = false; // we are out of leds
	delay(1000); // servo time to move
#ifdef SERIAL // menu only needet when serial is needet
	Serial.println("main();");
#endif
}

// XXX fill drunken sailor + final_countdown
void final_contdown(){// Final Countdown
	// A T1
	ag(t_viertel);
	d(t_achtel);
	bd(t_sechzehntel);
	bc(t_sechzehntel);
	bd(t_viertel);
	ag(t_achtel);
	// T2
	adis(t_viertel);
	d(t_achtel);
	bdis(t_sechzehntel);
	bd(t_sechzehntel);
	bdis(t_achtel);
	bd(t_achtel);
	bc(t_viertel);
	// T3
	bc(t_viertel);
	d(t_achtel);
	bdis(t_sechzehntel);
	bd(t_sechzehntel);
	bdis(t_viertel);
	ag(t_viertel);
	// T4
	af(t_viertel);
	d(t_achtel);
	bc(t_sechzehtnel);
	aais(t_sechzehntel);
	bc(t_achtel);
	aais(t_achtel);
	aa(t_achtel);
	bc(t_achtel);
	// T5
	ag(t_viertel);
	d(t_achtel);
	bd(t_sechzehntel);
	bc(t_sechzehntel);
	bd(t_viertel);
	ag(t_viertel);
	// T6
	
}

void drunken_sailor(){// Drunken Sailor
	for(int i = 0; i<3; i++){
		aa(t_achtel);	
	}
}

void bohemian_rhapsody(){
	/*
	 *
	 *		      _B_o_h_e_m_i_a_n_ _R_h_a_p_s_o_d_y_
	 *
	 *		       Words and Music by Freddy Mercury
	 *
	 *
	 *    (c) 1975 (Renewed 2003) B. FELDMAN & CO., LTD., Trading As TRIDENT MUSIC
	 *	 All Rights Controlled and Administered by GLENWOOD MUSIC CORP.
	 *    All Rights Reserved  International Copyright Secured  Used by Permission
	 *
	*/
	// S4 intro T1
	led0(1); led3(1); led6(1);
	bais(t_achtel); d(t_neuanschlag);
	led1(1);
	bais(t_achtel); d(t_neuanschlag);
	led7(1);
	bais(t_achtel); d(t_neuanschlag);
	led4(1);
	bais(t_viertel); d(t_neuanschlag);
	led0(0);
	bais(t_viertel); bais(t_achtel);
	led7(0);
	servo(10);
	// T2
	d(t_achtel);
	led2(1);
	bg(t_achtel); d(t_neuanschlag);
	led5(1);
	bg(t_achtel);
	led3(0);
	bais(t_achtel);
	led6(0);
	bg(t_achtel);
	led2(0);
	ba(t_sechzehntel);
	led2(1);
	bg(t_sechzehntel); bg(t_achtel);
	led2(0);
	servo(-10);
	// T3
	d(t_viertel);
	led1(1); led7(1);
	ba(t_achtel); d(t_neuanschlag);
	led4(0);
	ba(t_sechzehntel); d(t_neuanschlag);
	led3(1);
	ba(t_sechzehntel);
	led5(0);
	bais(t_viertel);
	led0(0);
	ba(t_achtel);
	led6(1);
	af(t_achtel); d(t_neuanschlag);
	led7(0); led2(1);
	servo(20);
	// T4
	af(t_achtel);
	led4(1);
	bais(t_achtel); d(t_neuanschlag);
	led6(0);
	bais(t_achtel);
	led0(1);
	bg(t_achtel);
	led7(1);
	bais(t_achtel);
	led2(0);
	ba(t_achtel);
	led3(0);
	bf(t_viertel);
	servo(-20);
	// S5 A T1
	bd(t_achtel); d(t_neuanschlag);
	led1(0);
	bd(t_achtel); d(t_neuanschlag);
	led3(1);
	bd(t_achtel); d(t_neuanschlag);
	led2(1);
	bd(t_halbe); d(t_neuanschlag);
	led4(0);
	bd(t_achtel); d(t_neuanschlag);
	led5(1);
	servo(5);
	// T2
	bd(t_achtel); d(t_neuanschlag);
	led3(0);
	bd(t_achtel);
	led1(1);
	bdis(t_achtel);
	led0(0);
	bf(t_viertel);
	led2(0);
	aais(t_viertel);
	led3(1);
	bg(t_achtel);
	led7(0);
	servo(-25);
	// T3
	bg(t_viertel);
	led1(0);
	if(refrain_play() == 0){
		bg(t_viertel); bg(t_halbe); d(t_neuanschlag);
	}else{
		ag(t_achtel);
		led5(0); led6(1);
		aais(t_achtel);
		led6(0); led7(1);
		bc(t_achtel);
		led7(0); led0(1);
		bdis(t_achtel);
		led0(0); led1(1);
		aais(t_achtel);
		led1(0); led5(1);
		bdis(t_achtel);
	}
	led2(1);
	servo(40);
	// T4
	bg(t_achtel); d(t_neuanschlag);
	led3(0);
	bg(t_achtel); d(t_neuanschlag);
	led4(1);
	bg(t_achtel); d(t_neuanschlag);
	led5(0);
	bg(t_viertel); d(t_neuanschlag);
	led6(1);
	bg(t_viertel); bg(t_achtel);
	led1(1);
	servo(-15);
	// T5
	bf(t_achtel); d(t_neuanschlag);
	led0(1);
	bf(t_achtel);
	led2(0);
	bg(t_achtel);
	led0(0);
	bf(t_sechzehntel);
	led5(1);
	bd(t_sechzehntel);
	led0(1);
	if(refrain_play() == 0){
		bc(t_halbe);
		servo(30);
		// T6
		ah(t_viertel);
		led2(1);
		aais(t_viertel);
		led0(0);
		aa(t_viertel);
		led4(0);
		aais(t_viertel);
		servo(50);
		// T7
		ah(t_viertel);
		led2(0);
		aais(t_viertel);
		led4(1);
		aa(t_viertel);
		led0(1);
		aais(t_viertel);
	}else{
		bc(t_achtel);
		led0(0);
		agis(t_achtel); d(t_neuanschlag);
		led1(0);
		agis(t_achtel);
		led2(1);
		aais(t_achtel);
		led3(1);
		servo(60);
		// T6
		bh(t_achtel); d(t_neuanschlag);
		led5(0);
		bh(t_achtel);
		led7(1);
		bais(t_viertel);
		led6(0);
		ba(t_achtel); d(t_neuanschlag);
		led5(1);
		ba(t_achtel);
		led4(0);
		bais(t_viertel);
		led6(1);
		servo(10);
		// T7
		led1(1);
		bh(t_achtel); d(t_neuanschlag);
		led2(0);
		bh(t_achtel);
		led3(0);
		bais(t_viertel);
		led0(1);
		ba(t_achtel); d(t_neuanschlag);
		led7(0);
		ba(t_achtel);
		led4(1);
		bais(t_viertel);
	}
	led6(0);
	servo(30);
	// T8
	bg(t_achtel); d(t_neuanschlag);
	led2(1);
	bg(t_achtel); d(t_neuanschlag);
	led3(1);
	bg(t_achtel); d(t_neuanschlag);
	led4(0);
	bg(t_achtel);
	led0(0);
	bf(t_viertel);
	led3(0);
	bais(t_viertel);
	led7(1);
	servo(0);
	// S6 T1
	be(t_achtel); d(t_neuanschlag);
	led7(0);
	be(t_achtel); d(t_neuanschlag);
	led6(1);
	be(t_achtel);
	led4(1);
	bf(t_achtel); d(t_neuanschlag);
	led5(0);
	bf(t_achtel);
	led6(0);
	af(t_achtel); d(t_neuanschlag);
	led5(1);
	af(t_achtel);
	led0(1);
	adis(t_achtel);
	led1(0);
	servo(10);
	// T2
	if(refrain_play() == 1){
		// T2
		bc(t_halbe);
		led0(1);
		af(t_zweisechzehntel);
		led1(1);
		aa(t_zweisechzehntel);
		led2(1);
		bc(t_zweisechzehntel);
		led3(1);
		bdis(t_zweisechzehntel); bdis(t_achtel);
		led3(0);
		aa(t_achtel);
		led1(0);
		aais(t_achtel); d(t_neuanschlag);
	}else{

	}
	led0(1);
	servo(-20);
	// T3
	if(refrain_play()==1){
		aais(t_achtel);
		led1(1);
		af(t_achtel);
		led2(1);
		aais(t_achtel);
		led3(1);
		bd(t_achtel);
		led3(0);
		bg(t_achtel);
		led4(0);
		af(t_achtel);
		led4(1);
		bf(t_achtel);
		led1(0);
		af(t_achtel);
		servo(5);
		// T4
		led0(1);
		bd(t_achtel);
		led1(1);
		af(t_achtel);
		led2(1);
		aais(t_achtel);
		led3(1);
		bd(t_achtel);
		led3(0);
		bg(t_achtel);
		led4(0);
		af(t_achtel);
		led4(1);
		bf(t_achtel);
		led1(0);
		af(t_achtel);
	}else{
		 // T2
		bc(t_halbe); bc(t_viertel);
		led1(1);
		aa(t_achtel);
		led5(0);
		aais(t_achtel); d(t_neuanschlag);
		led4(0);
		servo(60);
		// T3
		aais(t_halbe);
		led4(1);
		d(t_halbe);
		led1(0);
		servo(-5);
		// T4
		d(t_ganze);
	}
	led5(1);
	servo(0);
	// B T5
	bd(t_achtel); d(t_neuanschlag);
	led7(1);
	bd(t_achtel); bd(t_halbe);
	led3(1);
	d(t_achtel);
	led4(0);
	aais(t_achtel); d(t_neuanschlag);
	led2(0);
	servo(40);
	// T6
	bc(t_achtel);
	led1(1);
	bd(t_achtel); d(t_neuanschlag);
	led0(0);
	bd(t_halbe);
	led6(1);
	d(t_achtel);
	led4(1);
	bd(t_sechzehntel); d(t_neuanschlag);
	led7(0);
	bd(t_sechzehntel);
	led5(0);
	servo(-20);
	// T7
	bdis(t_achtel);
	led1(0);
	bf(t_achtel);
	led4(0);
	bdis(t_achtel);
	led2(1);
	bd(t_achtel);
	led5(1);
	bc(t_viertel); d(t_neuanschlag);
	led7(1);
	bc(t_achtel);
	led6(0);
	bd(t_achtel);
	led3(0);
	servo(20);
	// T8
	bdis(t_achtel);
	led0(1);
	bf(t_achtel);
	led6(1);
	bdis(t_achtel);
	led4(1);
	bd(t_achtel);
	led6(0);
	bc(t_viertel);
	led2(0);
	d(t_viertel);
	led7(0);
	servo(-5);
	// S7 T1
	bd(t_achtel); d(t_neuanschlag);
	led2(1);
	bd(t_achtel); bd(t_halbe); d(t_neuanschlag);
	led7(1);
	bd(t_achtel);
	led4(0);
	bf(t_achtel);
	led6(1);
	servo(35);
	// T2
	ba(t_achtel); ba(t_sechzehntel);
	led4(1);
	bg(t_sechzehntel); d(t_neuanschlag);
	led6(0);
	bg(t_halbe);
	led1(1);
	d(t_achtel);
	led0(0);
	bg(t_achtel);
	led3(1);
	bais(t_achtel); d(t_neuanschlag);
	led2(0);
	bais(t_achtel); d(t_neuanschlag);
	led4(0);
	bais(t_achtel); d(t_neuanschlag);
	led6(1);
	bais(t_achtel); d(t_neuanschlag);
	led7(0);
	bais(t_achtel); d(t_neuanschlag);
	led0(1);
	bg(t_achtel);
	led4(1);
	bdis(t_achtel);
	led5(0);
	bd(t_achtel);
	led7(1);
	servo(20);
	// T4
	bc(t_ganze);
	led3(0);
	servo(60);
	// T5
	bg(t_achtel); d(t_neuanschlag);
	led7(0);
	bg(t_achtel); bg(t_halbe);
	led2(1);
	bf(t_achtel);
	led1(0);
	bg(t_sechzehntel);
	led7(1);
	bgis(t_sechzehntel);
	led7(0);
	servo(40);
	// T6
	bg(t_halbe); bg(t_viertel);
	led5(1);
	d(t_achtel);
	led6(0);
	bg(t_sechzehntel); d(t_neuanschlag);
	led7(1);
	bg(t_sechzehntel);
	led1(1);
	servo(0);
	// T7
	bgis(t_achtel); bgis(t_sechzehntel);
	led0(0);
	bg(t_achtel); d(t_neuanschlag);
	led2(0);
	bg(t_achtel);
	led3(1);
	bf(t_achtel); d(t_neuanschlag);
	led4(0);
	bf(t_viertel); bf(t_achtel);
	led6(1);
	aais(t_achtel); d(t_neuanschlag);
	led6(0);
	servo(20);
	// T8
	aais(t_achtel);
	led1(0);
	bf(t_achtel); d(t_neuanschlag);
	led7(0);
	bf(t_achtel);
	led2(1);
	bg(t_achtel); d(t_neuanschlag);
	led6(1);
	bg(t_achtel);
	led5(0);
	bgis(t_achtel); d(t_neuanschlag);
	led4(1);
	bgis(t_achtel);
	led3(0);
	bais(t_achtel);
	led5(0);
	servo(-10);
	// S8 T1
	bgis(t_achtel);
	led3(1);
	bg(t_viertel);
	led2(0);
	bf(t_sechzehntel);
	led7(1);
	bg(t_sechzehntel);
	led0(1);
	bais(t_viertel);
	led1(1);
	bf(t_sechzehntel);
	led6(0);
	bg(t_sechzehntel);
	led4(0);
	servo(10);
	// T2
	bdis(t_viertel); bdis(t_achtel);
	led5(1);
	aais(t_sechzehntel); d(t_neuanschlag);
	led2(1);
	aais(t_neuanschlag);
	led1(0);
	ah(t_achtel);
	led6(1);
	bcis(t_sechzehntel);
	led7(0);
	ah(t_achtel);
	led0(0);
	bcis(t_sechzehntel);
	led7(1);
	ah(t_achtel);
	led0(1);
	servo(-30);
	// T3
	if(refrain_play() == 0){
		bd(t_halbe); bd(t_viertel);
		d(t_viertel); 
	}else{
		ag(t_achtel);
		adis(t_achtel);
		ag(t_achtel);
		aais(t_achtel);
		bc(t_achtel);
		bdis(t_achtel);
		aais(t_achtel);
		bdis(t_achtel);
	}
	led2(0);
	servo(5);
	// T4
	if(refrain_play()==0){
		d(t_halbe);
	}else{
		aa(t_achtel);
		bdis(t_achtel);
		agis(t_achtel);
		bdis(t_achtel);
	}
	led1(1);
	servo(-10);
	// T5
	if(refrain_play()==0){
		d(t_ganze);
	}else{
		bd(t_achtel);
		af(t_achtel);
		aais(t_achtel);
		bd(t_achtel);
		bg(t_achtel);
		af(t_achtel);
		bf(t_achtel);
		af(t_achtel);
	}
	led7(0);
	servo(20);
	// T6
	if(refrain_play()==0){
		d(t_ganze);
	}else{
		bd(t_achtel);
		af(t_achtel);
		aais(t_achtel);
		bd(t_achtel);
		bg(t_achtel);
		af(t_achtel);
		bf(t_achtel);
		af(t_achtel);    
	}
	led4(1);
	servo(30);
	// D T7
	bd(t_achtel); d(t_neuanschlag);
	led5(0);
	bd(t_achtel); bd(t_halbe);
	led7(1);
	d(t_achtel);
	led3(0);
	aais(t_achtel); d(t_neuanschlag);
	led2(1);
	servo(-20);
	// T8
	bc(t_achtel);
	led1(0);
	bd(t_achtel); d(t_neuanschlag);
	led0(0);
	bd(t_halbe);
	led7(0);
	d(t_achtel);
	led0(1);
	bd(t_achtel);
	led5(1);
	servo(10);
	// S9 T1
	bdis(t_achtel);
	led4(0);
	bf(t_achtel);
	led7(1);
	bdis(t_achtel);
	led3(1);
	bd(t_achtel);
	led1(1);
	bc(t_viertel); d(t_neuanschlag);
	led0(0);
	bc(t_achtel);
	led1(0);
	bd(t_achtel);
	led4(1);
	servo(20);
	// T2
	bdis(t_achtel);
	led3(0);
	bf(t_achtel);
	led5(0);
	bdis(t_achtel);
	led7(0);
	bd(t_achtel);
	led1(1);
	bc(t_viertel);
	led2(0);
	d(t_viertel);
	led3(1);
	servo(-10);
	// T3
	bd(t_achtel); d(t_neuanschlag);
	led4(0);
	bd(t_viertel);
	led5(1);
	bc(t_sechzehntel);
	led6(0);
	aais(t_sechzehntel);
	led0(1);
	bc(t_achtel);
	led7(1);
	bd(t_achtel);
	led6(1);
	d(t_sechzehntel);
	led5(0);
	bf(t_achtel);
	led4(1);
	servo(0);
	// T4
	ba(t_achtel); ba(t_sechzehntel);
	led3(0);
	bg(t_sechzehntel); d(t_neuanschlag);
	led2(1);
	bg(t_halbe);
	led1(0);
	d(t_achtel);
	led1(1);
	bf(t_sechzehntel);
	led4(0);
	bg(t_sechzehntel);
	led5(1);
	servo(-5);
	// T5
	bais(t_achtel); d(t_neuanschlag);
	led3(1);
	bais(t_achtel); d(t_neuanschlag);
	led2(0);
	bais(t_achtel); d(t_neuanschlag);
	led4(1);
	bais(t_achtel); d(t_neuanschlag);
	led5(0);
	bais(t_achtel); d(t_neuanschlag);
	led3(0);
	bg(t_achtel);
	led4(0);
	bdis(t_achtel);
	led5(1);
	bd(t_achtel);
	led6(0);
	servo(30);
	// T6
	bc(t_ganze);
	led2(1);
	servo(5);
	// T7
	bg(t_achtel); d(t_neuanschlag);
	led1(0);
	bg(t_achtel); bg(t_halbe);
	led3(1);
	bf(t_achtel);
	led2(0);
	bg(t_sechzehntel);
	led1(1);
	bgis(t_sechzehntel);
	led0(0);
	servo(40);
	// T8
	bg(t_ganze);
	led5(1);
	servo(10);
	// S10 T1
	bgis(t_achtel); bgis(t_sechzehntel);
	led6(0);
	bg(t_sechzehntel); d(t_neuanschlag);
	led4(1);
	bg(t_achtel);
	led5(0);
	bf(t_achtel); d(t_neuanschlag);
	led6(1);
	bf(t_viertel); bf(t_achtel); bf(t_sechzehntel);
	led7(0);
	aais(t_sechzehntel); d(t_neuanschlag);
	led3(0);
	servo(50);
	// T2
	aais(t_achtel);
	led4(0);
	bf(t_achtel); d(t_neuanschlag);
	led2(1);
	bf(t_achtel);
	led0(1);
	bg(t_achtel); d(t_neuanschlag);
	led1(0);
	bg(t_sechzehntel);
	led5(1);
	bgis(t_sechzehntel); d(t_neuanschlag);
	led7(1);
	bgis(t_achtel); d(t_neuanschlag);
	led6(0);
	bgis(t_achtel);
	led3(1);
	bais(t_achtel);
	led4(1);
	servo(15);
	// E T3
	bgis(t_achtel); bgis(t_sechzehntel);
	led2(0);
	bg(t_sechzehntel); d(t_neuanschlag);
	led5(0);
	bg(t_viertel);
	led0(0);
	if(refrain_play() == 0){
		d(t_halbe);
	}else{
		aais(t_achtel);
		ad(t_achtel);
		aais(t_achtel);
		ad(t_achtel);
	}
	led1(1);
	servo(60);
	// T4
	if(refrain_play()==0){
		d(t_ganze);
	}else{
		adis(t_achtel);
		ac(t_achtel);
		adis(t_achtel);
		aais(t_achtel);
		bd(t_achtel);
		ac(t_achtel);
		bc(t_achtel);
		ac(t_achtel);
	}
	led6(1);
	servo(50);
	// T5
	if(refrain_play()==0){
		d(t_ganze);
		servo(40);
	}else{
		agis(t_achtel);
		af(t_achtel);
		bc(t_achtel);
		aais(t_achtel);
		bc(t_achtel);
		adis(t_achtel);
		bc(t_achtel);
		ad(t_achtel);
	}
	led7(0);
	// T6
	if(refrain_play()==0){
		d(t_ganze);
	}else{
		bais(t_achtel);
		ag(t_sechzehntel); d(t_neuanschlag);
		ag(t_sechzehntel);
		bais(t_achtel);
		ag(t_sechzehntel); d(t_neuanschlag);
		ag(t_sechzehntel);
		bais(t_achtel);
		ag(t_sechzehntel); d(t_neuanschlag);
		ag(t_sechzehntel);
		bais(t_achtel);
		ag(t_sechzehntel); d(t_neuanschlag);
		ag(t_sechzehntel);
	}
	led2(1);
	servo(30);
	// T7
	if(refrain_play()==0){
		d(t_ganze);
	}else{
		d(t_achtel);
		adis(t_achtel);
		bdis(t_achtel);
		adis(t_achtel);
		aais(t_achtel);
		ad(t_achtel);
		aais(t_achtel);
		ad(t_achtel);
	}	
	led1(0);
	servo(20);
	// S11 T1
	if(refrain_play()==0){
		d(t_ganze);
	}else{
		adis(t_achtel);
		ac(t_achtel);
		adis(t_achtel);
		aais(t_achtel); 
		bd(t_achtel);
		ac(t_achtel);
		bc(t_achtel);
		ac(t_achtel);
	}
	led5(1);
	servo(10);
	// T2
	if(refrain_play()==0){
		d(t_ganze);
	}else{
		agis(t_achtel);
		af(t_achtel);
		bc(t_achtel); d(t_neuanschlag);
		bc(t_achtel); d(t_neuanschlag);
		bc(t_achtel);
		adis(t_achtel);
		bc(t_achtel);
		ad(t_achtel);
	}
	led6(0);
	servo(0);
	// T3
	if(refrain_play()==0){
		d(t_ganze);
	}else{
		bcis(t_achtel); d(t_neuanschlag);
		bcis(t_sechzehntel); d(t_neuanschlag);
		bcis(t_sechzehntel); d(t_neuanschlag);
		bcis(t_achtel_t); d(t_neuanschlag);
		bcis(t_achtel_t); d(t_neuanschlag);
		bcis(t_achtel_t); d(t_neuanschlag);
		bcis(t_achtel_t); d(t_neuanschlag);
		bcis(t_achtel_t); d(t_neuanschlag);
		bcis(t_achtel_t); d(t_neuanschlag);
		bcis(t_achtel_t); d(t_neuanschlag);
		bcis(t_achtel_t); d(t_neuanschlag);
		bcis(t_achtel_t);
	}
	led0(1); led7(1);
	servo(-10);
	// F T4
	ae(t_viertel); d(t_neuanschlag);
	led0(1);
	ae(t_viertel); d(t_neuanschlag);
	led7(1);
	ae(t_viertel); d(t_neuanschlag);
	led1(1);
	ae(t_viertel); d(t_neuanschlag);
	led6(1);
	servo(30);
	// T5
	ae(t_viertel); d(t_neuanschlag);
	led2(0);
	ae(t_viertel); d(t_neuanschlag);
	led5(0);
	ae(t_viertel); d(t_neuanschlag);
	led3(0);
	ae(t_viertel);
	led4(0);
	servo(15);
	// T6
	if(refrain_play()==0){
		afis(t_viertel);
		acis(t_achtel); d(t_neuanschlag);
		acis(t_achtel);
		adis(t_achtel);
		ae(t_achtel);
	}else{
		bd(t_viertel);
		bcis(t_achtel); d(t_neuanschlag);
		bcis(t_achtel);
		bc(t_achtel); d(t_neuanschlag);
		bc(t_achtel);
		bcis(t_achtel); d(t_neuanschlag);
		bcis(t_achtel);
	}
	led5(1);
	servo(55);
	// T7
	if(refrain_play()==0){
		af(t_viertel);
		ae(t_achtel); d(t_neuanschlag);
		ae(t_achtel);
		adis(t_viertel);
		ae(t_achtel); d(t_neuanschlag);
		ae(t_achtel);
	}else{
		bd(t_achtel); d(t_neuanschlag);
		bd(t_achtel);
		bcis(t_achtel); d(t_neuanschlag);
		bcis(t_achtel);
		bc(t_viertel);
		be(t_achtel); d(t_neuanschlag);
		be(t_achtel);
	}
	led2(1);
	servo(42);
	// S12 T1
	if(refrain_play()==0){
		aa(t_viertel);
		ae(t_achtel); d(t_neuanschlag);
		ae(t_achtel);
		aa(t_viertel);
		ae(t_achtel); d(t_neuanschlag);
		ae(t_achtel);
	}else{
		ba(t_viertel);
		be(t_achtel); d(t_neuanschlag);
		be(t_achtel);
		ba(t_viertel);
		be(t_achtel); d(t_neuanschlag);
		be(t_achtel);
	}
	led6(0);
	servo(35);
	// T2
	if(refrain_play()==0){
		adis(t_achtel); d(t_neuanschlag);
		adis(t_achtel);
		ae(t_viertel);
		afis(t_viertel);
		ae(t_viertel);
	}else{
		bdis(t_achtel); d(t_neuanschlag);
		bdis(t_achtel);
		be(t_viertel);
		bfis(t_viertel);
		be(t_viertel);
	
	}
	led1(0);
	servo(40);
	// T3
	if(refrain_play()==0){
		agis(t_achtel); d(t_neuanschlag);
		agis(t_achtel); d(t_neuanschlag);
		agis(t_achtel); d(t_neuanschlag);
		agis(t_achtel); d(t_neuanschlag);
		agis(t_viertel); d(t_neuanschlag);
		agis(t_viertel);
	}else{
		bf(t_achtel); d(t_neuanschlag);
		bf(t_achtel);
		bdis(t_achtel);
		bf(t_achtel);
		bg(t_viertel); d(t_neuanschlag);
		bg(t_viertel);
	}
	led6(1);
	servo(-5);
	// T4
	if(refrain_play()==0){
		ag(t_achtel); d(t_neuanschlag);
		ag(t_achtel);
		ae(t_achtel); d(t_neuanschlag);
		ae(t_achtel);
	}else{
		bg(t_achtel); d(t_neuanschlag);
		bg(t_achtel); d(t_neuanschlag);
		bg(t_achtel); d(t_neuanschlag);
		bg(t_achtel); d(t_neuanschlag);
		bg(t_viertel); d(t_neuanschlag);
		bg(t_viertel);
	}
	led1(1);
	servo(0);
	// T5
	if(refrain_play()==0){
		aa(t_achtel);
	}else{
		ba(t_achtel);
	}
	led0(0);
	d(t_achtel);
	led7(0);
	be(t_achtel); d(t_neuanschlag);
	led3(1);
	be(t_achtel);
	led2(0);
	bf(t_achtel);
	led4(1);
	be(t_achtel);
	led5(0);
	aa(t_achtel); d(t_neuanschlag);
	led1(0);
	aa(t_achtel);
	led0(1);
	servo(-30);
	// T6
	aais(t_achtel); d(t_neuanschlag);
	led7(1);
	aais(t_achtel);
	led6(0);
	be(t_achtel); d(t_neuanschlag);
	led2(1);
	be(t_achtel);
	led5(1);
	bf(t_achtel);
	led3(0);
	be(t_achtel);
	led4(0);
	aa(t_achtel); d(t_neuanschlag);
	led1(1);
	aa(t_achtel);
	led6(1); 
	servo(-23);
	// T7
	aais(t_achtel);
	aa(t_achtel);
	if(refrain_play()==0){
		be(t_achtel); d(t_neuanschlag);
		be(t_achtel);
		bf(t_achtel);
		be(t_achtel);
		bd(t_achtel);
		bc(t_achtel);
	}else{
		d(t_neuanschlag);
		aa(t_achtel); d(t_neuanschlag);
		aa(t_achtel);
		aais(t_achtel);
		aa(t_achtel);
		ag(t_achtel);
		af(t_achtel);
	}
	led5(0);
	servo(9);
	// T8
	if(refrain_play()==0){
		ah(t_halbe);
	}else{
		ae(t_achtel);
		aais(t_achtel);
		bais(t_achtel); d(t_neuanschlag);
		bais(t_achtel);
	}
	led4(1);
	bg(t_viertel); bg(t_achtel);
	led3(1);
	bdis(t_achtel);
	led4(0);
	servo(0);
	// S13 T1
	bh(t_halbe);
	led1(1);
	servo(70);
	// T2
	if(refrain_play()==0){
		ah(t_viertel);
		aais(t_achtel); d(t_neuanschlag);
		aais(t_achtel);
		aa(t_achtel);
		aais(t_achtel); d(t_neuanschlag);
		aais(t_achtel);
	}else{
		ah(t_viertel);
		aais(t_viertel);
		aa(t_viertel);
		aais(t_viertel);
	}
	led3(0);
	servo(30);
	// T3
	ah(t_viertel);
	led5(1);
	aais(t_achtel); d(t_neuanschlag);
	led7(0);
	aais(t_achtel);
	led6(1);
	aa(t_viertel);
	led4(1);
	aais(t_viertel);
	led6(0);
	servo(40);
	// T4
	bdis(t_viertel); d(t_neuanschlag);
	led2(0);
	bdis(t_achtel); d(t_neuanschlag);
	led3(1);
	bdis(t_achtel); d(t_neuanschlag);
	led6(1);
	bdis(t_viertel); d(t_neuanschlag);
	led5(0);
	bdis(t_viertel); d(t_neuanschlag);
	led2(1);
	servo(20);
	// T5
	bdis(t_achtel); d(t_neuanschlag);
	led1(0);
	bdis(t_achtel); d(t_neuanschlag);
	led3(0);
	bdis(t_viertel); d(t_neuanschlag);
	led7(1);
	bdis(t_viertel); d(t_neuanschlag);
	led3(1);
	bdis(t_viertel);
	led4(0);
	servo(-20);
	// T6
	bgis(t_viertel); d(t_neuanschlag);
	led1(1);
	bgis(t_achtel); d(t_neuanschlag);
	led0(0);
	bgis(t_achtel);
	led5(1);
	bg(t_viertel); d(t_neuanschlag);
	led3(0);
	bg(t_achtel); d(t_neuanschlag);
	led7(0);
	bg(t_achtel);
	led4(1);
	servo(10);
	// T7
	bf(t_achtel);
	led5(0);
	bdis(t_achtel);
	led7(1);
	bd(t_achtel);
	led1(0);
	bc(t_achtel);
	led0(1);
	aais(t_achtel); d(t_neuanschlag);
	led1(1);
	aais(t_achtel); d(t_neuanschlag);
	led2(0);
	aais(t_achtel); d(t_neuanschlag);
	led3(1);
	aais(t_achtel);
	led4(0);
	servo(-25);
	// T8
	bc(t_achtel); 
	led2(1);
	bdis(t_achtel);
	led3(0);
	aais(t_achtel);
	led5(1);
	bdis(t_achtel); 
	led7(0);
	aa(t_achtel);
	led4(1);
	bdis(t_achtel); 
	led6(0);
	agis(t_achtel); 
	led7(1);
	bdis(t_achtel);
	led0(0);
	servo(5);
	// G S14 T1
	if(refrain_play()==0){
		aais(t_achtel);
		bc(t_achtel);
		aais(t_viertel); d(t_neuanschlag);
		aais(t_achtel); d(t_neuanschlag);
		aais(t_achtel); d(t_neuanschlag); // as#
		aais(t_viertel); // as#
	}else{
		ah(t_viertel);
		aais(t_viertel);
		aa(t_viertel);
		aais(t_viertel);
	}
	led3(1);
	servo(-30);
	// T2
	if(refrain_play()==0){
		ah(t_achtel); d(t_neuanschlag);
		ah(t_achtel);
		aais(t_achtel); d(t_neuanschlag);
		aais(t_achtel); d(t_neuanschlag);
		aais(t_viertel); // as#
		hh(t_viertel);
	}else{
		ah(t_viertel);
		aais(t_viertel);
		aa(t_viertel);
		aais(t_viertel);
	}
	led2(0);
	servo(15);
	// T3
	adis(t_viertel);
	led5(0);
	hh(t_viertel);
	led6(1);
	if(refrain_play()==0){
		bg(t_viertel); bg(t_achtel);
		bf(t_achtel); d(t_neuanschlag);
	}else{
		adis(t_viertel); adis(t_achtel);
		af(t_achtel); d(t_neuanschlag);
	}
	led0(1);
	servo(0);
	// T4
	if(refrain_play()==0){
		bf(t_achtel);
		bg(t_achtel);
		bgis(t_achtel);
		bg(t_achtel);
		bf(t_achtel);
	}else{
		af(t_achtel);
		ag(t_achtel);
		agis(t_achtel);
		ag(t_achtel);
		af(t_achtel);
	}
	led7(0);
	bais(t_achtel); d(t_neuanschlag);
	led6(0);
	bais(t_achtel); d(t_neuanschlag);
	led7(1);
	bais(t_achtel);
	led5(1);
	servo(-20);
	// T5
	bais(t_achtel);
	led4(0);
	d(t_achtel);
	led0(0);
	hh(t_achtel);
	led3(0);
	adis(t_achtel);
	led2(1);
	hh(t_achtel);
	led1(0);
	if(refrain_play()==0){
		bf(t_achtel); d(t_neuanschlag);
	}else{
		af(t_achtel); d(t_neuanschlag);
	}
	led2(0);
	servo(40);
	// T6
	if(refrain_play()==0){
		bf(t_achtel);
		bg(t_achtel);
		bgis(t_achtel);
		bg(t_achtel);
		bf(t_achtel);
	}else{
		af(t_achtel);
		ag(t_achtel);
		agis(t_achtel);
		ag(t_achtel);
		af(t_achtel);
	}
	led3(1);
	bais(t_achtel); d(t_neuanschlag);
	led2(1);
	bais(t_achtel); d(t_neuanschlag);
	led7(0);
	bais(t_achtel);
	led3(0);
	servo(20);
	// T7
	bais(t_achtel);
	led6(1);
	d(t_achtel);
	led4(1);
	hh(t_achtel);
	led5(0);
	adis(t_achtel);
	led1(1);
	hh(t_achtel);
	led2(0);
	if(refrain_play()==0){
		bf(t_achtel); d(t_neuanschlag);
	}else{
		af(t_achtel); d(t_neuanschlag);
	}
	led3(1);
	servo(50);
	// T8
	if(refrain_play()==0){
		bf(t_achtel);
		bg(t_achtel);
		bgis(t_achtel);
		bg(t_achtel);
		bf(t_achtel);
	}else{
		af(t_achtel);
		ag(t_achtel);
		agis(t_achtel);
		ag(t_achtel);
		af(t_achtel);
	}
	led4(0);
	bdis(t_achtel); d(t_neuanschlag);
	led5(1);
	bdis(t_achtel); d(t_neuanschlag);
	led6(0);
	bdis(t_achtel);
	led7(1);
	servo(12);
	// S15 T1
	if(refrain_play()==0){
		bf(t_achtel);
		bg(t_achtel);
		bgis(t_achtel);
		bg(t_achtel);
		bf(t_achtel);
	}else{
		bd(t_achtel);
		bdis(t_achtel);
		bf(t_achtel);
		bdis(t_achtel);
		bd(t_achtel);
	}
	led1(0);
	bdis(t_achtel);
	led2(1);
	if(refrain_play()==0){
		d(t_neuanschlag);
		bdis(t_achtel); d(t_neuanschlag);
		bdis(t_achtel);
	}else{
		ag(t_achtel);
		af(t_achtel);
	}
	led0(1);
	servo(20);
	// T2
	if(refrain_play()==0){
		bf(t_achtel);
		bg(t_achtel);
		bgis(t_achtel);
		bg(t_achtel);
		bf(t_achtel);
		bais(t_viertel); bais(t_achtel);
	}else{
		agis(t_achtel);
		ag(t_achtel);
		aais(t_achtel);
		agis(t_achtel);
		bc(t_achtel);
		aais(t_achtel);
		bdis(t_achtel); d(t_neuanschlag);
		bdis(t_achtel);
	}
	led3(0);
	servo(40);
	// T3
	d(t_achtel);
	led1(1);
	bais(t_viertel);
	led4(1);
	bfis(t_viertel);
	led6(1);
	bf(t_achtel);
	led5(0);
	bcis(t_viertel);
	led7(0);
	servo(-30);
	// T4
	afis(t_viertel);
	led3(1);
	ag(t_viertel);
	led2(0);
	agis(t_viertel);
	led0(0);
	aais(t_viertel);
	led7(1);
	servo(-15);
	// H T5
	bcis(t_viertel);
	led5(1);
	bd(t_viertel);
	led4(0);
	bdis(t_achtel);
	led6(0);
	aais(t_achtel); d(t_neuanschlag);
	led4(1);
	aais(t_achtel); d(t_neuanschlag);
	led3(0);
	aais(t_achtel);
	led2(1);
	servo(0);
	// T6
	bc(t_achtel);
	led1(0);
	aais(t_achtel); d(t_neuanschlag);
	led5(0);
	aais(t_achtel); d(t_neuanschlag);
	led3(1);
	aais(t_achtel);
	led4(0);
	bc(t_achtel);
	led0(1);
	aais(t_achtel);
	led2(0);
	aais(t_achtel); d(t_neuanschlag);
	led4(1);
	aais(t_achtel);
	led3(0);
	servo(15);
	// T7
	bc(t_achtel);
	led1(1);
	aais(t_achtel);
	led6(1);
	agis(t_achtel);
	led3(1);
	ag(t_achtel);
	led0(0);
	af(t_viertel);
	led7(0);
	aais(t_viertel);
	led4(0);
	servo(30);
	// T8
	if(refrain_play()==0){
		bdis(t_viertel); bdis(t_achtel); d(t_neuanschlag);
		bdis(t_achtel); d(t_neuanschlag);
		bdis(t_viertel); d(t_neuanschlag);
		bdis(t_achtel); d(t_neuanschlag);
		bdis(t_achtel);
	}else{
		d(t_neuanschlag);
		aais(t_viertel); aais(t_achtel); d(t_neuanschlag);
		aais(t_achtel);
		bc(t_viertel); d(t_neuanschlag);
		bc(t_achtel); d(t_neuanschlag);
		bc(t_achtel); d(t_neuanschlag);
	}
	led4(1);
	servo(45);
	// S16 T1
	if(refrain_play()==0){
		bd(t_achtel); d(t_neuanschlag);
		bd(t_achtel); d(t_neuanschlag);
		bd(t_achtel); d(t_neuanschlag);
		bd(t_achtel);
		bg(t_viertel); d(t_neuanschlag);
		bg(t_viertel);
	}else{
		bc(t_achtel); d(t_neuanschlag);
		bc(t_achtel); d(t_neuanschlag);
		bc(t_achtel); d(t_neuanschlag);
		bc(t_achtel);
		bd(t_viertel); d(t_neuanschlag);
		bd(t_viertel);
	}
	led7(1);
	servo(60);
	// T2
	agis(t_viertel); agis(t_achtel); d(t_neuanschlag);
	led3(0);
	agis(t_achtel);
	led4(0);
	af(t_viertel);
	led2(1);
	if(refrain_play()==0){
		aais(t_viertel);
	}else{
		ag(t_achtel);
	}
	led6(0);
	servo(45);
	// T3
	if(refrain_play()==0){
		bc(t_viertel); bc(t_achtel);
		aais(t_achtel);
		agis(t_viertel);
		aais(t_viertel);
	}else{
		agis(t_viertel); agis(t_achtel);
		ag(t_achtel);
		af(t_viertel);
		ag(t_viertel);
	}
	led7(0);
	servo(30);
	// T4
	if(refrain_play()=0){
		bais(t_ganze);
	}else{
		bf(t_ganze);
	}
	led0(1);
	servo(-30);
	// T5
	if(refrain_play()==0){
		bais(t_ganze);
	}else{
		bf(t_ganze);
	}
	led4(1);
	servo(0);
	// T6
	bdis(t_viertel_t);
	led5(1);
	ag(t_viertel_t);
	led3(1);
	agis(t_viertel_t);
	led1(0);
	aais(t_achtel); aais(t_sechzehntel);
	led2(0);
	bc(t_sechzehntel);
	led0(0);
	bd(t_sechzehntel);
	led6(1);
	bdis(t_achtel); bdis(t_sechzehntel); d(t_neuanschlag);
	led5(0);
	servo(-22);
	// T7
	bdis(t_viertel_t);
	led7(1);
	ag(t_viertel_t);
	led3(0);
	bc(t_achtel_t); 
	led3(1);
	aais(t_achtel_t); d(t_neuanschlag);
	led4(0);
	aais(t_achtel_t);
	led5(1);
	bc(t_achtel_t);
	led7(0);
	aais(t_achtel_t); aais(t_viertel);
	led6(0);
	servo(11);
	// S17 T1
	bdis(t_viertel_t);
	led7(1);
	ag(t_viertel_t);
	led4(1);
	agis(t_viertel_t);
	led5(0);
	aais(t_achtel); aais(t_sechzehntel);
	led2(1);
	bc(t_sechzehntel);
	led3(0);
	bd(t_sechzehntel);
	led4(0);
	bdis(t_achtel); bdis(t_sechzehntel);
	led3(1);
	servo(-33);
	// T2
	bf(t_viertel_t);
	led4(1);
	agis(t_viertel_t);
	led2(0);
	aais(t_viertel_t);
	led1(1);
	bc(t_achtel_t);
	led4(0);
	bd(t_achtel_t);
	led5(1);
	bc(t_achtel_t); bc(t_viertel);
	led7(0);
	servo(10);
	// T3
	d(t_viertel_t);
	led6(1);
	agis(t_viertel_t); d(t_neuanschlag);
	led4(1);
	agis(t_viertel_t); d(t_neuanschlag);
	led2(1);
	agis(t_viertel_t);
	led0(1);
	ag(t_viertel_t);
	led1(0);
	af(t_viertel_t);
	led3(0);
	servo(20);
	// T4
	ag(t_viertel_t);
	led5(0);
	af(t_viertel_t); d(t_neuanschlag);
	led7(1);
	af(t_viertel_t); d(t_neuanschlag);
	led6(0);
	af(t_sechzehntel);
	led5(1);
	adis(t_achtel); adis(t_sechzehntel); d(t_neuanschlag);
	led4(0);
	adis(t_viertel);
	led3(1);
	servo(5);
	// T5
	ad(t_achtel); ad(t_sechzehntel);
	led2(0);
	adis(t_sechzehntel);
	led1(1);
	af(t_viertel);
	led0(0);
	aais(t_achtel_t); d(t_neuanschlag);
	led1(0);
	aais(t_achtel_t);
	led2(1);
	agis(t_achtel_t);
	led1(1);
	aais(t_achtel_t);
	led0(1);
	agis(t_achtel_t);
	led3(0);
	af(t_achtel_t);
	led2(0);
	d(t_achtel);
	led1(0);
	bd(t_achtel); bd(t_viertel); // vib. XXX 
	led4(1);
	servo(-35);
	// T6
	d(t_viertel_t);
	led3(1);
	agis(t_viertel_t); d(t_neuanschlag);
	led2(1);
	agis(t_viertel_t); d(t_neuanschlag);
	led1(1);
	agis(t_viertel_t);
	led5(0);
	ag(t_viertel_t); d(t_neuanschlag);
	led4(0);
	ag(t_viertel_t); d(t_neuanschlag);
	led3(0);
	servo(45);
	// T7
	ag(t_viertel_t);
	led2(0);
	af(t_viertel_t); d(t_neuanschlag);
	led6(1);
	af(t_viertel_t); d(t_neuanschlag);
	led5(1);
	af(t_viertel_t);
	led4(1);
	adis(t_viertel_t); d(t_neuanschlag);
	led3(1);
	adis(t_viertel_t);
	led2(1);
	servo(0);
	// S18 T1
	agis(t_achtel); agis(t_sechzehntel);
	led7(0);
	aais(t_sechzehntel);
	led6(0);
	bc(t_viertel);
	led5(0);
	adis(t_viertel);
	led4(0);
	d(t_viertel);
	led3(0);
	servo(44);
	// T2
	agis(t_ganze);
	led1(0);
	servo(0);
	// T3
	ag(t_achtel); ag(t_sechzehntel);
	led7(1);
	af(t_sechzehntel); d(t_neuanschlag);
	led3(1);
	af(t_halbe); af(t_viertel);
	led4(1);
	servo(-11);
	// T4
	d(t_viertel_t);
	led2(0);
	agis(t_viertel_t); d(t_neuanschlag);
	led4(0);
	agis(t_viertel_t); d(t_neuanschlag);
	led1(1);
	agis(t_viertel_t);
	led0(0);
	ag(t_viertel_t); d(t_neuanschlag);
	led2(1);
	ag(t_viertel_t); d(t_neuanschlag);
	led7(0);
	servo(23);
	// T5
	ag(t_achtel); ag(t_sechzehntel);
	led3(0);
	af(t_sechzehntel); d(t_neuanschlag);
	led1(0);
	af(t_viertel); d(t_neuanschlag);
	led5(1);
	af(t_achtel); af(t_sechzehntel);
	led2(0);
	ag(t_sechzehntel); d(t_neuanschlag);
	led3(1);
	ag(t_viertel);
	led0(1);
	servo(-29);
	// T6
	adis(t_viertel); d(t_neuanschlag);
	led4(1);
	adis(t_achtel); d(t_neuanschlag);
	led5(0);
	adis(t_achtel); d(t_neuanschlag);
	led1(1);
	adis(t_viertel);
	led0(0);
	af(t_halbe);
	led2(1);
	servo(13);
	// T7
	adis(t_viertel); d(t_neuanschlag);
	led1(0);
	adis(t_achtel); d(t_neuanschlag);
	led3(0);
	adis(t_achtel); d(t_neuanschlag);
	led4(0);
	adis(t_viertel);
	led5(1);
	af(t_viertel_t);
	led3(1);
	adis(t_viertel_t);
	led7(1);
	ad(t_viertel_t);
	led6(1);
	servo(25);
	// T8
	if(refrain_play()==0){
		adis(t_viertel);
		hh(t_halbe); hh(t_viertel);
	}else{
		bdis(t_viertel_t);
		ag(t_viertel_t);
		agis(t_viertel_t);
		aais(t_achtel); aais(t_sechzehntel);
		bc(t_sechzehntel);
		bd(t_sechzehntel);
		bdis(t_achtel); bdis(t_sechzehntel); d(t_neuanschlag);
	}
	led7(0);
	servo(0);
	// S19 T1
	bdis(t_viertel_t);
	led5(0);
	ag(t_viertel_t);
	led4(1);
	agis(t_viertel_t);
	led3(0);
	aais(t_achtel_t);
	led0(1);
	bc(t_achtel_t);
	led1(1);
	aais(t_achtel_t); d(t_neuanschlag);
	led2(0);
	aais(t_viertel);
	led4(0);
	servo(-9);
	// T2
	bdis(t_viertel_t);
	led2(1);
	ag(t_viertel_t);
	led0(0);
	agis(t_viertel_t);
	led3(1);
	aais(t_achtel); aais(t_sechzehntel);
	led1(0);
	bc(t_sechzehntel);
	led4(1);
	bd(t_sechzehntel);
	led2(0);
	bdis(t_achtel); bdis(t_sechzehntel);
	led5(1);
	servo(14);
	// T3
	bf(t_viertel_t);
	led3(0);
	aa(t_viertel_t);
	led5(0);
	aais(t_viertel_t);
	led1(1);
	bc(t_achtel); bc(t_sechzehntel);
	led3(1);
	bd(t_sechzehntel);
	led4(0);
	be(t_sechzehntel);
	led5(1);
	bf(t_achtel); bf(t_sechzehntel);
	led6(0);
	servo(42);
	// T4
	ag(t_achtel_t);
	led7(1);
	agis(t_achtel_t);
	led4(1);
	aais(t_achtel_t); aais(t_achtel_t);
	led3(0);
	agis(t_achtel_t);
	led2(1);
	aais(t_achtel_t);
	led1(0);
	bc(t_achtel); bc(t_sechzehntel);
	led0(1);
	aais(t_sechzehntel);
	led1(1);
	bc(t_sechzehntel);
	led2(0);
	bcis(t_achtel); bcis(t_sechzehntel);
	led7(0);
	servo(-8);
	// T5
	adis(t_achtel_t);
	led4(0);
	ae(t_achtel_t);
	led6(1);
	afis(t_achtel_t);
	led7(1);
	agis(t_achtel_t);
	led5(0);
	aa(t_achtel_t);
	led6(0);
	bc(t_achtel_t);
	led2(1);
	bcis(t_achtel_t);
	led3(1);
	bc(t_achtel_t);
	led4(1);
	bcis(t_achtel_t);
	led1(0);
	bdis(t_sechzehntel);
	led2(0);
	be(t_achtel); be(t_sechzehntel);
	led3(0);
	servo(72);
	// T6
	agis(t_achtel_t);
	led6(1);
	aais(t_achtel_t);
	led5(1);
	bc(t_achtel_t);
	led7(0);
	bcis(t_achtel_t);
	led5(0);
	bdis(t_achtel_t);
	led1(1);
	bf(t_achtel_t);
	led2(1);
	bfis(t_achtel_t);
	led3(1);
	bf(t_achtel_t);
	led5(1);
	bfis(t_achtel_t);
	led0(0);
	bgis(t_sechzehntel);
	led1(0);
	bais(t_achtel); bais(t_sechzehntel);
	led2(0);
	servo(-34);
	// T7
	hh(t_achtel_t);
	led3(0);
	ac(t_achtel_t);
	led7(1);
	ad(t_achtel_t);
	led4(0);
	ac(t_achtel_t);
	led5(0);
	ad(t_achtel_t);
	led4(1);
	adis(t_achtel_t);
	led1(1);
	af(t_achtel_t);
	led7(0);
	adis(t_achtel_t);
	led3(1);
	af(t_achtel_t);
	led4(0);
	ag(t_sechzehntel);
	led7(1);
	agis(t_achtel); agis(t_sechzehntel);
	led1(0);
	servo(0);
	// T8
	hh(t_achtel_t);
	led6(0);
	ac(t_achtel_t);
	led4(1);
	ad(t_achtel_t);
	led3(0);
	ac(t_achtel_t);
	led5(1);
	ad(t_achtel_t);
	led2(1);
	adis(t_achtel_t);
	led4(0);
	af(t_achtel_t);
	led3(1);
	adis(t_achtel_t);
	led1(1);
	af(t_achtel_t);
	led2(0);
	ag(t_achtel_t);
	led5(0);
	agis(t_achtel_t);
	led6(1);
	ag(t_achtel_t);
	led3(0);
	servo(17);
	// S20 T1
	agis(t_achtel_t);
	led0(1);
	aais(t_achtel_t);
	led1(0);
	bc(t_achtel_t);
	led2(1);
	aais(t_achtel_t);
	led3(1);
	bd(t_achtel_t);
	led4(1);
	aais(t_achtel_t);
	led5(1);
	bf(t_achtel);
	led6(0);
	aais(t_achtel);
	led7(0);
	servo(0);
	// K T2
	if(refrain_play()==0){ // XXX make other voices
		bg(t_halbe);
		bgis(t_halbe);
	}else{
		bdis(t_halbe);
		ah(t_halbe);
	}
	led2(0);
	servo(62);
	// T3
	if(refrain_play()==0){
		bg(t_halbe); d(t_neuanschlag);
		bg(t_viertel);
		bdis(t_viertel);
	}else{
		bc(t_halbe);
		ah(t_viertel);
		ag(t_viertel);
	}
	led3(0);
	servo(34);
	// T4
	if(refrain_play()==0){
		bg(t_viertel);
		bdis(t_viertel);
	}else{
		ah(t_viertel);
		ag(t_viertel);
	}
	led4(0);
	if(refrain_play()==0){ 
		bd(t_viertel);
		ag(t_viertel);
	}else{
		agis(t_viertel);
		adis(t_viertel);
	}
	led2(1);
	servo(-12);
	// T5
	if(refrain_play()==0){
		afis(t_halbe);
		aais(t_halbe);
	}else{
		ad(t_halbe);
		ag(t_halbe);
	}
	led5(0);
	servo(-30);
	// T6
	if(refrain_play()==0){
		bc(t_halbe);
		bg(t_halbe);
	}else{
		agis(t_halbe);
		bdis(t_halbe);
	}
	led4(1);
	servo(57);
	// T7
	if(refrain_play()==0){
		bgis(t_achtel);
		bg(t_achtel);
		bdis(t_achtel);
		aais(t_achtel); d(t_neuanschlag);
		aais(t_achtel); d(t_neuanschlag);
		aais(t_achtel); aais(t_viertel);
	}else{
		d(t_achtel);
		bc(t_achtel);
		bg(t_achtel);
		bc(t_achtel);
		d(t_achtel);
		ag(t_achtel);
		bg(t_achtel);
		ag(t_achtel);
	}
	led1(1);
	servo(-29);
	// T8
	if(refrain_play()==0){
		bg(t_achtel);
		bf(t_achtel);
		bdis(t_achtel);
		bd(t_achtel);
		aais(t_halbe);
	}else{
		d(t_achtel);
		bc(t_achtel);
		bdis(t_achtel);
		bg(t_achtel);
		d(t_achtel);
		ag(t_achtel);
		aais(t_achtel);
		bd(t_achtel);
	}
	led3(1);
	servo(-87);
	// S21 T1 L
	if(refrain_play()==0){
		bg(t_achtel);
		bdis(t_achtel);
		bf(t_achtel_t);
		bg(t_achtel_t);
		bdis(t_achtel_t);
		ah(t_halbe);
	}else{
		bc(t_achtel); d(t_neuanschlag);
		bc(t_achtel);
		bdis(t_achtel);
		bg(t_achtel);
		aais(t_halbe);
	}
	led5(1);
	servo(28);
	// T2
	if(refrain_play()==0){
		bg(t_achtel);
		bf(t_achtel); d(t_neuanschlag);
		bf(t_achtel);
		bdis(t_achtel); d(t_neuanschlag);
		bdis(t_achtel);
		bc(t_viertel);
		bd(t_achtel);
	}else{
		ah(t_ganze);
	}
	led0(0);
	servo(64);
	// T3
	if(refrain_play()==1){
		bdis(t_halbe);
	}else{
		ag(t_achtel);
		adis(t_achtel);
		ag(t_achtel);
		bc(t_achtel);
	}
	led2(0);
	if(refrain_play()==0){
		bc(t_achtel);
		bd(t_achtel);
		bc(t_achtel);
		bd(t_achtel);
	}else{
		agis(t_achtel);
		bd(t_achtel);
		agis(t_achtel);
		bd(t_achtel);
	}
	led4(0);
	servo(-20);
	// T4
	if(refrain_play()==0){
		aais(t_achtel);
	}else{
		ag(t_achtel);
	}
	led1(0);
	adis(t_achtel);
	led2(1);
	ag(t_achtel);
	led3(0);
	aais(t_achtel);
	led4(1);
	if(refrain_play()==0){
		bc(t_achtel);
		bd(t_achtel);
		bc(t_achtel);
		bd(t_achtel);
	}else{
		aa(t_achtel);
		bd(t_achtel);
		aa(t_achtel);
		bd(t_achtel);
	}
	led5(0);
	servo(5);
	// T5
	if(refrain_play()==0){
		aais(t_achtel);
	}else{
		af(t_achtel);
	}
	led6(1);
	ad(t_achtel);
	led0(1);
	af(t_achtel);
	led1(1);
	aais(t_achtel);
	led0(0);
	if(refrain_play()==0){
		d(t_neuanschlag); aais(t_achtel);
		bc(t_achtel);
		aais(t_achtel);
		bc(t_achtel);
	}else{
		af(t_achtel);
		bc(t_achtel);
		af(t_achtel);
		bc(t_achtel);
	}
	led3(1);
	servo(23);
	// T6
	if(refrain_play()==0){
		ag(t_achtel);
	}else{
		ae(t_achtel);
	}
	led1(0);
	ac(t_achtel);
	led5(1);
	ae(t_achtel);
	led3(0);
	ag(t_achtel);
	led7(1);
	if(refrain_play()==0){
		aais(t_achtel);
	}else{
		ae(t_achtel);
	}
	led5(0);
	agis(t_achtel); d(t_neuanschlag);
	led7(0);
	agis(t_achtel);
	led7(1);
	ag(t_achtel);
	led5(1);
	servo(6);
	// T7
	if(refrain_play()==0){
		d(t_neuanschlag);
		ag(t_achtel);
	}else{
		ac(t_achtel);
	}
	led7(0);
	adis(t_achtel);
	led3(1);
	ac(t_achtel);
	led5(0);
	hh(t_achtel);
	led1(1);
	ha(t_achtel);
	led3(0);
	agis(t_achtel);
	led0(1);
	bf(t_achtel);
	led1(0);
	af(t_achtel);
	led0(0);
	servo(9);
	// T8
	if(refrain_play()==1){
		aais(t_achtel); d(t_neuanschlag);
		aais(t_achtel);
		aa(t_achtel); d(t_neuanschlag);
		led1(1);
		aa(t_achtel);
		agis(t_viertel);
		bf(t_viertel);
	}else{
		bd(t_achtel);
		bf(t_achtel);
		bc(t_achtel);
		led1(1);
		bf(t_achtel);
		ah(t_achtel);
		bf(t_achtel);
		aais(t_achtel);
	}
	led1(0);
	servo(42);
	// T9
	if(refrain_play()==1){
		aa(t_ganze); aa(t_ganze);
	}else{
		af(t_ganze); af(t_ganze);
	}
} // end bohemain_rhapsody

//EOF
/* ------------ DOKU ------------

NAME
	DISCO - ein Programm mit Musik, blinkenden LEDs und vielem mehr

DESCRIPTION
	Ein Programm fuer Arduino, das Musik spielen und LEDs blinken lassen kann.

OPTIONS
	Man kann folgende Optionen im <config> Berich aktivieren (auskommentieren):

	 #define SERIAL
		aktiviert die serialle Schnittstelle zur Kommunikation mit einem
		Computer. Man kann auf der seriellen Schnittstelle folgende Zeichen
		senden:
			0       aktiviert die Musik mit LEDs
			1       aktiviert nur die LEDs
			2       aktiviert nur die Musik
			
	 #define TOENE_TIEFER
		spielt alle Toene eine Oktave tiefer, damit die Toene nicht so 
		peipsig sind.

	 #define SERVO
	 int servo_null = 75;
		aktiviert den Servo, sodass dieser sich bewegt und setzt die Nullposition
		des Servos.

	 #define PLAY_REFRAIN
		spielt immer den Refrain
		
	 #define PLAY_REFRAIN_NOT
		spielt den Refrain nie
		
	 #define PLAY_REFRAIN_SWICH
		spielt den Refrain wenn der Schalter umgelgt ist
	
	Weitere Konfigurationsmoeglichkeiten:

	 int tonepin = 3;
		der Pin fuer den Lautsprecher.

	 int servopin = 4;
		der Pin, an dem der Gelbe Pin des Servos angeschlossen ist.

	 int tasterpin = 5;
		der Pin, der von dem Taster auf Masse gezogen wird.

	 int lichtschrankenpin = 6;
		der Pin, an dem der Fotowiederstand angeschlossen ist.

	 int refrain_schalter = 7;
		der Pin, an dem der Schalter fuer die Auswahl angeschlossen ist.

	 int onboardled = 13;
		der Pin der Status-LED oder Pin 13 LED.
 
	 int speed = 1;
	 int schlaege_pro_minute = 73;
	 int t_neuanschlag = 20;
		Definiton der Geschwindigkeit, der Schlaege pro Minute und der Zeit (in ms)
		wie lange gewartet werden soll, um einen erneuten Tonanschlag zu speilen.

	 int pin_74HC595_data = 8;
	 int pin_74HC595_stcp = 9;
	 int pin_74HC595_clock = 10;
	 int pin_74HC595_reset = 11;
		Definiton der Pins, die zu dem Schieberegister fuehren.
		data ist am Schieberegister Pin 14, stcp Pin 12, clock
		ist Pin 13 und reset ist an Pin 10 angeschlossen.

SEE ALSO
	<https://www.arduino.cc>
	
COPYRIGHT
	Copyright (C) 2020  Johannes Schmatz, Alwin Fronius

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

AUTHORS
	Johannes Schmatz
	Alwin Fronius

BUGS
	There is no bug in this software, everything you find is a feature.
	If you find a feature in this software please report it in the file BUGS.txt
	(create it if it doesn't exists).

SOURCES
	<https://de.wikipedia.org/wiki/Frequenzen_der_gleichstufigen_Stimmung>
	<https://m.youtube.com/users/Themorpheus407/>
	<https://de.wikipedia.org/wiki/C++>
	ISO/IEC 14882:2017
	Datei -> Beispiele -> 04. Communication -> ReadASCIIString
	Datei -> Beispiele -> 04. Communication -> ASCIITable
	Datei -> Beispiele -> Servo -> Knob

*/
//DOKUEOF
//// syntax for some text editors
// vim: ts=8 sw=8 syntax=arduino
