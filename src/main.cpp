#include <AccelStepper.h>

// Initialize Stepper Library
AccelStepper stepperX(AccelStepper::DRIVER,5,7);

// Define Arduino Pins
#define X_home_switch 3
#define X_end_switch 2
#define vry A1

// Stepper motor speed values
const int set_accel_X = 800;
const int set_max_speed_X = 2000;
const int set_speed = 2000;
float x_dir = 500.0;
float x_speed = 0;

//Time values
const unsigned int bounce_period = 2000; 
unsigned long futureMillis;
float secs = 0;

//Counters and bools
long int counterX = 0;
byte x_home_on = 1;
byte x_end_on = 1;
long int end_limit = 450000;
int flipper = 1;

// Serial parameters
const int BUFFER_SIZE = 100;
byte buf[BUFFER_SIZE];

void move_stepperX(float Xspeed_input){
  stepperX.enableOutputs();
  stepperX.setSpeed(Xspeed_input);  
  stepperX.runSpeed();
  if (counterX % 500 == 0) {
    Serial.println(counterX);
  }
}

void update_counter(float Xspeed_input) {
  if (Xspeed_input > 0) {
    counterX -= 1;
  } else if (Xspeed_input < 0) {
    counterX += 1;
  }
}

void flip_switch() {
  Serial.println("flipped direction");
  if (flipper == 1) {
    flipper = -1;
  } else if (flipper == -1) {
    flipper = 1;
  }
}

void set_motorX(){
  stepperX.setAcceleration(set_accel_X);
  stepperX.setMaxSpeed(set_max_speed_X);
  stepperX.disableOutputs();
}

void move_stepper_by_time(float speed = set_speed, unsigned long time = 1){
  futureMillis = millis() + time;
  while (millis() < futureMillis){
    move_stepperX(speed);
    update_counter(speed);
  }
}

void go_home(){
  bool reached_home = false;
  while (reached_home == false) {
    x_home_on= digitalRead(X_home_switch);
    if (x_home_on == 0){
      x_home_on = true;
      counterX = 0;
      Serial.println(counterX);
      move_stepper_by_time(-set_speed, 7125);
      Serial.print("distance: ");
      Serial.println(counterX);
      break;
    } else {
      move_stepperX(set_speed);
      update_counter(set_speed);
    }
  } 
  Serial.println(counterX);
}

void limitbounce(byte whichend = 0){
  //  0 is home, 1 is end
  Serial.println("Reached:");
  Serial.println(whichend);
  if (whichend == 0){
    move_stepper_by_time(-set_speed, bounce_period);
  } else if (whichend == 1) {
    move_stepper_by_time(set_speed, bounce_period);
  }
}

void setup(){  
   Serial.begin(9600);
   pinMode(X_home_switch,INPUT_PULLUP);
   pinMode(X_end_switch,INPUT_PULLUP);
   set_motorX();
   counterX = 0;
   Serial.println("initializing");
   go_home();
}

void loop(){  
  x_home_on= digitalRead(X_home_switch);
  x_end_on= digitalRead(X_end_switch);
  x_dir= analogRead(vry);
  if (x_home_on == 0) {
    limitbounce(0);
  } else if (x_end_on == 0) {
    limitbounce(1);
  } else {
    
    if (counterX >= end_limit and flipper == 1) {
      flip_switch();
    } else if (counterX <= 84200 and flipper == -1) {
      flip_switch();
    }
  
    if (Serial.available() > 0) {
      secs = Serial.readBytesUntil('\n', buf, BUFFER_SIZE);
      move_stepper_by_time(flipper*-1*set_speed, 52*secs);
      secs = 0;
    }
  }
}