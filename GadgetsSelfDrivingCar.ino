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
#define TURN_THRESHOLD 95 //
#define TURN_MINIMUM 40 //
#define PING_ITERATIONS 5

#define Forward 0
#define Turn 1
#define Turn_left 2
#define Turn_right 3
#define Reverse 4
#define Reverse_right 5
#define Reverse_left 6
#define Backward 7

 int state = 0;
 bool MOVEMENT = false;
 int left_dist;
 int previous_front;
 int front_dist;
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
  left_dist = left.convert_cm(left.ping_median(PING_ITERATIONS));
  if (left_dist == 0)
    left_dist = 999;

  previous_front = front_dist;
  //front_dist = front.convert_cm(front.ping_median(PING_ITERATIONS));
  front_dist = front.ping_cm();
  if (front_dist == 0)
    front_dist = 999;

  if (previous_front != front_dist)
    MOVEMENT = true;
  else
    MOVEMENT = false;
    
  right_dist = right.convert_cm(right.ping_median(PING_ITERATIONS));
  if (right_dist == 0)
    right_dist = 999;
  
  switch (state) {
    case Forward:
      turn_center();
      drive_forward();
      if(front_dist < TURN_THRESHOLD && front_dist > TURN_MINIMUM)
        state = Turn;
      else if(front_dist < TURN_MINIMUM)
        state = Reverse;
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
        state = Reverse_right;
      break;
      
    case Turn_right:
      turn_right();
      if(front_dist > TURN_THRESHOLD)
        state = Forward;
      else if (front_dist < TURN_MINIMUM)
        state = Reverse_left;
      break;
      
    case Reverse:
      if(left_dist > right_dist)
        state = Reverse_right;
      else
        state = Reverse_left;
      break;
      
    case Reverse_right:
      turn_left();
      drive_backward();
      if (front_dist > TURN_THRESHOLD || !MOVEMENT)
        state = Forward;        
      break;
      
    case Reverse_left:
      turn_right();
      drive_backward();
      if (front_dist > TURN_THRESHOLD || !MOVEMENT)
        state = Forward;  
      break;
      
    case Backward:
      break;
  }

  /*if(front_dist > TURN_THRESHOLD) {
    turn_center();
    drive_forward();
  }
  else if(front_dist > TURN_MINIMUM) {  // We have room to turn
    if(left_dist > right_dist){
      turn_left();
    }
    else {
      turn_right();
    }
  }
  else {  // We don't have room to complete a turn
    if()
  }*/
  
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
