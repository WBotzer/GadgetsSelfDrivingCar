// --------------------------------------------------------------------------- Libraries
#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <NewPing.h>
#include <PrintEx.h>//Debugging

// --------------------------------------------------------------------------- Constants
#define MAX_DISTANCE 300 //centimeters range 2 - 400
#define SPEED_1 85 //Approx. 3V to motors
//Do not operate front except at SPEED_1
//Rear motor can be operated at other speeds sparingly as motor failure can occur
#define SPEED_2 170 //Approx. 6V to motors
#define TURBO 255 //Approx. 9V to motors
#define TURN_THRESHOLD 90 //
 

// --------------------------------------------------------------------------- Motors
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *frontMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rearMotor = AFMS.getMotor(2);

// --------------------------------------------------------------------------- Sensors
NewPing front(5,4,MAX_DISTANCE);
NewPing left(13,12,MAX_DISTANCE);
NewPing right(9,8,MAX_DISTANCE);

// --------------------------------------------------------------------------- Setup
StreamEx mySerial = Serial;//Debugging

void setup() {
	// Setup motors
	AFMS.begin();
	frontMotor->setSpeed(SPEED_1);//Do not alter frontMotor speed
	rearMotor->setSpeed(SPEED_1);//Operate above SPEED_1 sparingly
  Serial.begin(9600);//For Debugging
}

// --------------------------------------------------------------------------- Loop
void loop() { 
  int front_dist = front.ping_cm();
  int left_dist = left.ping_cm();
  int right_dist = right.ping_cm();
  //Debugging Code  
  mySerial.printf("%l, %c, %r \n",left_dist,front_dist,right_dist);
	//!Debugging Code
}

// --------------------------------------------------------------------------- Detect
// TBD
//Methods for NewPing objects
//sonar.ping(); - Send a ping, returns the echo time in microseconds or 0 (zero) if no ping echo within set distance limit
//sonar.ping_in(); - Send a ping, returns the distance in inches or 0 (zero) if no ping echo within set distance limit
//sonar.ping_cm(); - Send a ping, returns the distance in centimeters or 0 (zero) if no ping echo within set distance limit
//sonar.ping_median(iterations); - Do multiple pings (default=5), discard out of range pings and return median in microseconds
//sonar.convert_in(echoTime); - Converts microseconds to distance in inches
//sonar.convert_cm(echoTime); - Converts microseconds to distance in centimeters
//sonar.ping_timer(function); - Send a ping and call function to test if ping is complete.
//sonar.check_timer(); - Check if ping has returned within the set distance limit.
//timer_us(frequency, function); - Call function every frequency microseconds.
//timer_ms(frequency, function); - Call function every frequency milliseconds.
//timer_stop(); - Stop the timer.

// --------------------------------------------------------------------------- Drive

void motor_stop(){//Stops drive motor
	rearMotor->run(RELEASE);
	delay(25);
}

void drive_forward(){//Drive motor forward
	rearMotor->run(FORWARD);
}

void drive_backward(){//Drive motor backward
	rearMotor->run(BACKWARD);
}

void turn_center(){//Returns steering to center
	frontMotor->run(RELEASE);
	delay(25);
}

void turn_left(){//Turn left
	frontMotor->run(FORWARD);
}

void turn_right(){//Turn right
	frontMotor->run(BACKWARD);
}
