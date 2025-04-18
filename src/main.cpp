#include <Arduino.h>
#include <PS4Controller.h>
#include <ESP32Servo.h>
#define IN1 18
#define IN2 5
#define ENA 4
#define IN3 17
#define IN4 16
#define ENB 15
#define SERVO_X_pin 27
#define SERVO_Y1_pin 14
#define SERVO_Y2_pin 12
#define SERVO_Z_pin 13
int SERVO_X_ANGLE = 90;
int SERVO_Y1_ANGLE = 90;
int SERVO_Y2_ANGLE = 90;
int SERVO_Z_ANGLE = 90;
Servo SERVO_X;
Servo SERVO_Y1;
Servo SERVO_Y2;
Servo SERVO_Z;
// put function declarations here:


// dicerotion 
void carMovement();
void carMovementX(int[]);
enum direction
{
  forward,
  reverse,
  stop
};
void Movement(int,int, direction);
void armMovement();
void armAngles();


void setup() {
  pinMode(2, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  PS4.begin("1a:2b:3c:01:01:01");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  /*
  SG90 servo -> 500us and 2400us
  MG995 servo -> 1000us and 2000us
  */
  SERVO_X.attach(SERVO_X_pin, 500, 2400); 
  SERVO_Y1.attach(SERVO_Y1_pin, 500, 2400); 
  SERVO_Y2.attach(SERVO_Y2_pin, 500, 2400);
  SERVO_Z.attach(SERVO_Z_pin, 500, 2400);
  armMovement();
  Serial.begin(115200);
}

void loop() {
  if (PS4.isConnected()) {
    digitalWrite(2,HIGH);
    carMovement();
    armAngles();
  }
  else {
    digitalWrite(2,LOW);
  }
}


void carMovement() {
  if(PS4.data.analog.button.r2)
  {
    int speed = abs(PS4.data.analog.button.r2);
    int speeds[2] = {speed, speed};
    carMovementX(speeds);
    Movement(speeds[0], speeds[1],direction::forward);
  }
  else if (PS4.data.analog.button.l2)
  {
    int speed = abs(PS4.data.analog.button.l2);
    int speeds[2] = {speed, speed}; 
    carMovementX(speeds);
    Movement(speeds[0], speeds[1],direction::reverse);
  }
  else
  {
    Movement(0, 0,direction::stop);
  }
}

void carMovementX(int speeds[]) {
  if (PS4.data.analog.stick.lx>=5)
  {
  int speedMinus = map(abs(PS4.data.analog.stick.lx),5,128,0,speeds[0]);
  speeds[1] -= speedMinus; 
  }
  else if (PS4.data.analog.stick.lx<=-5)
  {
  int speedMinus = map(abs(PS4.data.analog.stick.lx),5,128,0,speeds[1]);
  speeds[0] -= speedMinus;
  }
}

void Movement(int speedLeft, int speedRight, direction dir) {
  if(speedLeft > 255) speedLeft = 255;
  if(speedRight > 255) speedRight = 255;
  if(speedLeft < 0) speedLeft = 0;
  if(speedRight < 0) speedRight = 0;
  if(dir == direction::forward) // forward
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  else if(dir == direction::reverse) //reverse
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  else if(dir == direction::stop) // stop
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
  }
  analogWrite(ENA, speedLeft);
  analogWrite(ENB, speedRight);
  delay(20);
}


/* 
===================================================================================================
  Arm Logic


===================================================================================================
*/
void armAngles() {
  if (PS4.data.analog.stick.rx >= 5) SERVO_X_ANGLE += 2;
  else if (PS4.data.analog.stick.rx <= -5) SERVO_X_ANGLE -= 2;
  if( PS4.data.analog.stick.ry >= 5) {
    SERVO_Y1_ANGLE += 2;
    SERVO_Y2_ANGLE -= 2;}
  else if (PS4.data.analog.stick.ry <= -5) {
    SERVO_Y1_ANGLE -= 2;
    SERVO_Y2_ANGLE += 2;}
  if (PS4.event.button_down.r1) {
    while(PS4.event.button_down.r1) {
      SERVO_Z_ANGLE += 2;
      armMovement();
      delay(500);
    }
  }
  else if(PS4.event.button_down.l1){
    SERVO_Z_ANGLE=90;
  }
  if(PS4.event.button_down.triangle){
    SERVO_X_ANGLE = 90;
    SERVO_Y1_ANGLE = 90;
    SERVO_Y2_ANGLE = 90;
  }
  armMovement();
}
void armMovement(){
  SERVO_X.write(min(SERVO_X_ANGLE,180));
  SERVO_Y1.write(min(SERVO_Y1_ANGLE,180));
  SERVO_Y2.write(min(SERVO_Y2_ANGLE,180));
  SERVO_Z.write(min(SERVO_Z_ANGLE,180));
  delay(20);
}