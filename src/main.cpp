#include <Arduino.h>
#include <PS4Controller.h>
#define IN1 18
#define IN2 5
#define ENA 4
#define IN3 17
#define IN4 16
#define ENB 15
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
  Serial.begin(115200);
}

void loop() {
  if (PS4.isConnected()) {
    digitalWrite(2,HIGH);
    carMovement();
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


