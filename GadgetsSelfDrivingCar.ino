// --------------------------------------------------------------------------- Libraries
#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <NewPing.h>

// --------------------------------------------------------------------------- Constants
#define MAX_DISTANCE 300 //centimeters range 2 - 400
#define SPEED_1 85 //Approx. 3V to motors
//Do not operate front except at SPEED_1
//Rear motor can be operated at other speeds sparingly as motor failure can occur
#define SPEED_2 170 //Approx. 6V to motors
#define TURBO 255 //Approx. 9V to motors
#define TURN_THRESHOLD 95 //In cm
#define TURN_MINIMUM 40 //In cm
#define PING_ITERATIONS 5 //Number of pings for average
//For switch cases in loop()
#define Forward 0
#define Turn 1
#define Turn_left 2
#define Turn_right 3
#define Reverse 4
#define Reverse_right 5
#define Reverse_left 6
#define Backward 7

// --------------------------------------------------------------------------- Variables
int state = Forward;
bool MOVEMENT = false;
int count = 100;
int left_dist;
int previous_front = 0;
int front_dist = 0;
int right_dist;	

// --------------------------------------------------------------------------- Motors
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *frontMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rearMotor = AFMS.getMotor(2);

// --------------------------------------------------------------------------- Sensors
NewPing front(5,4,MAX_DISTANCE);
NewPing right(13,12,MAX_DISTANCE);
NewPing left(9,8,MAX_DISTANCE);

// --------------------------------------------------------------------------- Setup
void setup() {
	// Setup motors
	AFMS.begin();
	frontMotor->setSpeed(SPEED_1);//Do not alter frontMotor speed
	rearMotor->setSpeed(SPEED_1);//Operate above SPEED_1 sparingly
  Serial.begin(9600);
}

// --------------------------------------------------------------------------- Loop
void loop() {

  //ECC
	left_dist = left.ping_cm();
	if (left_dist == 0){
		left_dist = 999;
	}
	right_dist = right.ping_cm();
	if (right_dist == 0){
		right_dist = 999;
	}
	
	front_dist = front.ping_cm();
  if (abs(previous_front - front_dist) > 3) {
    count = 100;
  }
  else
    count --;
  previous_front = front_dist;
	if (front_dist == 0){
		front_dist = 999;
	}
 Serial.println(count);
  //!ECC
  
	switch (state) {
		case Forward:
			turn_center();
			drive_forward();
			if(front_dist < TURN_THRESHOLD && front_dist > TURN_MINIMUM)
				state = Turn;
			else if(front_dist < TURN_MINIMUM || count <= 0){
				count = 100;
				state = Reverse;
			}
			break;
      
		case Turn:
			if(left_dist > right_dist)
				state = Turn_left;
			else
				state = Turn_right;
			break;
      
		case Turn_left:
			turn_left();
			if(front_dist > TURN_THRESHOLD)
				state = Forward;
			else if (front_dist < TURN_MINIMUM)
				state = Reverse;
			break;
      
		case Turn_right:
			turn_right();
			if(front_dist > TURN_THRESHOLD)
				state = Forward;
			else if (front_dist < TURN_MINIMUM)
				state = Reverse;
			break;
      
		case Reverse:
			if(left_dist < right_dist)
				state = Reverse_right;
			else
				state = Reverse_left;
			break;
      
		case Reverse_right:
			turn_left();
			drive_backward();
			if (front_dist > TURN_THRESHOLD)
				state = Forward;        
			break;
      
		case Reverse_left:
			turn_right();
			drive_backward();
			if (front_dist > TURN_THRESHOLD)
				state = Forward;  
			break;
      
		default:
			motor_stop();
			turn_center();
			count=0;
	}
}

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
