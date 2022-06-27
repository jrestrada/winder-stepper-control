#include <AccelStepper.h>

AccelStepper stepperX(AccelStepper::DRIVER,5,7);

#define X_home_switch 3
#define X_end_switch 2
#define vry A1

const int set_accel_X = 800;
const int set_max_speed_X = 2000;
int set_speed = 2000;
float x_dir = 500.0;
float x_speed = 0;
byte x_home_on = 1;
byte x_end_on = 1;
int counterX = 0;
unsigned long futureMillis;
const unsigned int bounce_period = 2000; 
const 

void move_stepperX(float Xspeed_input){
  stepperX.enableOutputs();
  stepperX.setSpeed(Xspeed_input);  
  stepperX.runSpeed();
  counterX -= Xspeed_input/abs(Xspeed_input);
}

void set_motorX(){
  stepperX.setAcceleration(set_accel_X);
  stepperX.setMaxSpeed(set_max_speed_X);
  stepperX.disableOutputs();
}

void move_stepper_by_time(float speed = set_speed, unsigned long time = bounce_period){
  futureMillis = millis() + time;
  while (millis() < futureMillis){
    move_stepperX(speed);
  }
}

void go_home(){
  bool reached_home = false;
  while (reached_home == false) {
    x_home_on= digitalRead(X_home_switch);
    if (x_home_on == 0){
      x_home_on = true;
      counterX = 0;
      move_stepper_by_time(-set_speed, 7125);
      Serial.print("distance :");
      Serial.print(counterX);
      break;
    } else {
      move_stepperX(set_speed);
    }
  } 
}

void limitbounce(byte whichend = 0){
  //  0 is home, 1 is end
  Serial.println("Reached:");
  Serial.println(whichend);
  counterX = 0;
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
    if (x_dir > 700){
      x_speed = -set_speed;
    }else if (x_dir <400 ){
      x_speed = set_speed;
    }else{
       x_speed = 0;
    }
  }
  move_stepperX(x_speed);
}