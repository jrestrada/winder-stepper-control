#include <AccelStepper.h>

AccelStepper stepperX(AccelStepper::DRIVER,6,5);
AccelStepper stepperY(AccelStepper::DRIVER,8,7); 

#define X_home_switch 9
#define X_end_switch 10
#define Y_home_switch 11
#define Y_end_switch 12


#define vrx A0
#define vry A1

const int set_accel_X = 1000;
const int set_max_speed_X = 5000;
const int set_accel_Y = 1000;
const int set_max_speed_Y = 5000;
const int set_speed = 100;

float x_dir = 500.0;
float y_dir = 500.0;
float x_speed = 0;
float y_speed = 0;

int counterX = 0;

void setup()
{  
   Serial.begin(9600);
   
   pinMode(X_home_switch,INPUT_PULLUP);
   pinMode(X_end_switch,INPUT_PULLUP);
   pinMode(Y_home_switch,INPUT_PULLUP);
   pinMode(Y_end_switch,INPUT_PULLUP);

   attachInterrupt(digitalPinToInterrupt(X_home_switch),stop_stepper_X,FALLING);
   attachInterrupt(digitalPinToInterrupt(X_end_switch),stop_stepper_X,FALLING);

   stepperX.setAcceleration(set_accel_X);
   stepperY.setAcceleration(set_accel_Y);

   counterX = 1;

   set_motorX();

}

void loop()
{  
//    if(counterX == 0)
//   {
//      home_all();
//   }
//   else if(counterX == 1){
     x_dir = analogRead(vry);
     if (x_dir > 700){
        x_speed = set_speed;
     }
     else if(x_dir < 400){
        x_speed = -1*set_speed;
     }
     else{
        x_speed = 0;
     }
     x_speed = map(x_dir,1023,0,-1*set_max_speed_X,set_max_speed_X);
      move_stepperX(x_speed);
     //Serial.println(x_speed);
//   }
}

void home_all(){
  // X-direction
  if(digitalRead(X_home_switch) == 1){
    stepperX.setAcceleration(set_accel_X);
    stepperX.setMaxSpeed(set_max_speed_X);
    stepperX.setSpeed(-1*set_speed);
    stepperX.runSpeed();
  }
  else if(digitalRead(X_home_switch) == 0){
    stop_stepper_X();
    counterX = 1;
  }
  
  
  //Y-direction
//  while(Y_home_switch == 1){
//    stepperY.setAcceleration(set_accel_Y);
//    stepperY.setMaxSpeed(set_max_speed_Y);
//    stepperY.setSpeed(-1*set_speed);
//    stepperY.runSpeed();
//  }  
}

void move_stepperX(float Xspeed_input){
  stepperX.enableOutputs();
  //stepperX.setAcceleration(set_accel_X);
  stepperX.setMaxSpeed(set_max_speed_X);
  stepperX.setSpeed(Xspeed_input);  
  stepperX.runSpeed();
}

void move_stepperY(float Yspeed_input){
  stepperY.enableOutputs();
  //stepperY.setAcceleration(set_accel_Y);
  stepperY.setMaxSpeed(set_max_speed_Y);
  stepperY.setSpeed(Yspeed_input);
  stepperY.runSpeed();  
}

void stop_stepper_X(){
  stepperX.setMaxSpeed(set_max_speed_X);
  stepperX.setSpeed(0);
  stepperX.runSpeed();
  stepperX.disableOutputs();
}
void stop_stepper_Y(){
  stepperY.setMaxSpeed(set_max_speed_Y);
  stepperY.setSpeed(0);
  stepperY.disableOutputs();   
}

void set_motorX(){
  stepperX.setAcceleration(set_accel_X);
  stepperX.setMaxSpeed(set_max_speed_X);
  stepperX.disableOutputs();
}
//The function runSpeed() should work at the speed set by setSpeed()
//The functions setMaxSpeed() and setAcceleration() work with the function run().

