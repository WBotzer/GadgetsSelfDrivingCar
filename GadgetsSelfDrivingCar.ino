// --------------------------------------------------------------------------- Libraries
#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <NewPing.h>

// --------------------------------------------------------------------------- Constants
#define MAX_DISTANCE 400 //centimeters range 2 - 400
#define SPEED 85 //Approx. 3V to motors at 85 range 0 - 255
//Operate above 85 sparingly
#define TURN_THRESHOLD 100 //In cm
#define TURN_MINIMUM 80 //In cm
#define SIDE_THRESHOLD 25
#define ADJ .75 //Percentage to adjust SPEED
//For switch cases in loop()
#define Forward 0
#define Turn 1
#define Turn_left 2
#define Turn_right 3
#define Reverse 4
#define Reverse_right 5
#define Reverse_left 6

// --------------------------------------------------------------------------- Variables
int state = Forward;
int front_dist = 0;
int right_dist = 0;
int left_dist = 0;

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
  frontMotor->setSpeed(SPEED);//Do not alter frontMotor speed
  rearMotor->setSpeed(SPEED);//Operate above SPEED sparingly
}

// --------------------------------------------------------------------------- Loop
void loop() {

  //ECC
  left_dist = left.ping_cm();
  right_dist = right.ping_cm();
  front_dist = front.ping_cm();
  
  if (right_dist == 0){
    right_dist = 999;
  }
  if (front_dist == 0){
    front_dist = 999;
  }
  if (left_dist == 0){
    left_dist = 999;
  }
  //!ECC
  
  switch (state) {
    case Forward:
      turn_center();
      drive_forward();
      if(front_dist < MAX_DISTANCE)
        rearMotor->setSpeed(SPEED);
      else 
        rearMotor->setSpeed(SPEED / ADJ);
      if(front_dist < TURN_THRESHOLD && front_dist > TURN_MINIMUM || left_dist < SIDE_THRESHOLD || right_dist < SIDE_THRESHOLD)
        state = Turn;
      else if(front_dist < TURN_MINIMUM){
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
      if(front_dist > TURN_THRESHOLD && right_dist > SIDE_THRESHOLD)
        state = Forward;
      else if (front_dist < TURN_MINIMUM)
        state = Reverse;
      break;
      
    case Turn_right:
      turn_right();
      if(front_dist > TURN_THRESHOLD  && left_dist > SIDE_THRESHOLD)
        state = Forward;
      else if (front_dist < TURN_MINIMUM)
        state = Reverse;
      break;
      
    case Reverse:
      rearMotor->setSpeed(SPEED);
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
