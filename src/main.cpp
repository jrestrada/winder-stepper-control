#include <AccelStepper.h>

// Initialize Stepper Library
AccelStepper stepperX(AccelStepper::DRIVER,5,7);

// Define Arduino Pins
#define home_switch 3
#define end_switch 2
#define vry A1

// Stepper motor speed values
// Speeds in the negative direction move the device towards the end switch
// Speeds in the positive direction move the device towards the home switch

const int set_accel = 800;
const int set_max_speed = 2000;
const int set_speed = 2000;
float speed = 0;

//Joystick Control Values
float dir = 500.0;

//Time values
const unsigned int limit_bounce_t = 2000; 
const unsigned int homing_bounce_t = 7125;
unsigned long future_millis;

//Movement Values
long int positionX = 0;
long int reel_limit_end = 450000;
long int reel_limit_home = 84200;
int current_direction = 1;
int motion_rate = 52;
float motion_cmd = 0;
byte home_on = 1;
byte end_on = 1;

// Serial parameters
const int BUFFER_SIZE = 100;
byte buf[BUFFER_SIZE];

void set_motorX(){
  stepperX.setAcceleration(set_accel);
  stepperX.setMaxSpeed(set_max_speed);
  stepperX.disableOutputs();
}

void update_position(float Xspeed_input) {
  if (Xspeed_input > 0) {
    positionX -= 1;
  } else if (Xspeed_input < 0) {
    positionX += 1;
  }
}

void move_stepperX(float Xspeed_input){
  stepperX.enableOutputs();
  stepperX.setSpeed(Xspeed_input);  
  stepperX.runSpeed();
  update_position(Xspeed_input);
  if (positionX % 500 == 0) {
    Serial.println(positionX);
  }
}

void reverse_direction() {
  if (current_direction == 1) {
    current_direction = -1;
  } else if (current_direction == -1) {
    current_direction = 1;
  }
}

void move_stepper_by_time(float speed = set_speed, unsigned long t = 1){
  future_millis = millis() + t;
  while (millis() < future_millis){
    move_stepperX(speed);
  }
}

void limitbounce(byte whichend = 0){
  Serial.print("Reached:");
  Serial.println(whichend);
  if (whichend == 0){
    move_stepper_by_time(-set_speed, limit_bounce_t);
  } else if (whichend == 1) {
    move_stepper_by_time(set_speed, limit_bounce_t);
  }
}

void home_position(){
  bool reached_home = false;
  while (reached_home == false) {
    home_on = digitalRead(home_switch);
    if (home_on == 0){
      reached_home = true;
      positionX = 0;
      Serial.println(positionX);
      move_stepper_by_time(-set_speed, homing_bounce_t);
      Serial.print("distance: ");
      Serial.println(positionX);
      break;
    } else {
      move_stepperX(set_speed);
    }
  } 
  reel_limit_home = positionX;
  Serial.println(positionX);
}

void setup(){  
   Serial.begin(9600);
   Serial.println("initializing");
   pinMode(home_switch,INPUT_PULLUP);
   pinMode(end_switch,INPUT_PULLUP);
   set_motorX();
   home_position();
}

void loop(){  
  home_on = digitalRead(home_switch);
  end_on = digitalRead(end_switch);
  dir = analogRead(vry);
  if (home_on == 0) {
    limitbounce(0);
  } else if (end_on == 0) {
    limitbounce(1);
  } else {
    
    if (positionX >= reel_limit_end and current_direction == 1) {
      reverse_direction();
    } else if (positionX <= reel_limit_home and current_direction == -1) {
      reverse_direction();
    }
  
    if (Serial.available() > 0) {
      motion_cmd = Serial.readBytesUntil('\n', buf, BUFFER_SIZE);
      move_stepper_by_time(-1*current_direction*set_speed, motion_rate*motion_cmd);
      motion_cmd = 0;
    }
  }
}