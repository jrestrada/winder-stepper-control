#include <AccelStepper.h>

AccelStepper stepperX(AccelStepper::DRIVER,5,7);

#define X_home_switch 3
#define X_end_switch 2
#define vry A1

const int set_accel_X = 800;
const int set_max_speed_X = 2000;
int set_speed = 1500;
float x_dir = 500.0;
float x_speed = 0;
byte x_home_on = 1;
byte x_end_on = 1;
int counterX = 0;

unsigned long futureMillis;
const unsigned long bounce_period = 2000;

void move_stepperX(float Xspeed_input){
  stepperX.enableOutputs();
  stepperX.setAcceleration(set_accel_X);
  stepperX.setMaxSpeed(set_max_speed_X);
  stepperX.setSpeed(Xspeed_input);  
  stepperX.runSpeed();
  counterX += Xspeed_input/abs(Xspeed_input);
}

void set_motorX(){
  stepperX.setAcceleration(set_accel_X);
  stepperX.setMaxSpeed(set_max_speed_X);
  stepperX.disableOutputs();
}

void setup(){  
   Serial.begin(9600);
   pinMode(X_home_switch,INPUT_PULLUP);
   pinMode(X_end_switch,INPUT_PULLUP);
   set_motorX();
   counterX = 0;
   Serial.print("initializing");
}

void loop(){  
  x_home_on= digitalRead(X_home_switch);
  x_end_on= digitalRead(X_end_switch);
  x_dir = analogRead(vry);
  if (x_home_on == 0) {
    Serial.println("Reached Home");
    Serial.println(counterX);
    counterX = 0;
    Serial.println(counterX);
    futureMillis = millis() + bounce_period;
    while (millis() < futureMillis){
      move_stepperX(-set_speed);
    }
  } else if (x_end_on == 0) {
    futureMillis = millis() + bounce_period;
    Serial.println("Reached End");
    Serial.println(counterX);
    while (millis() < futureMillis){
      move_stepperX(set_speed);
    }

  } else {
    if (x_dir > 700){
      //  x_speed = map(x_dir,1023,0,-1*set_max_speed_X,set_max_speed_X);
      x_speed = -set_speed;
    }else if (x_dir <400 ){
      //  x_speed = map(x_dir,1023,0,-1*set_max_speed_X,set_max_speed_X);
      x_speed = set_speed;
    }else{
       x_speed = 0;
    }
  }
  move_stepperX(x_speed);
}