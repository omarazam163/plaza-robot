#include <Arduino.h>
#include <Ps3Controller.h>
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
  Ps3.begin("00:00:00:00:00:00");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  Serial.begin(115200);
}

void loop() {
  if (Ps3.isConnected()) {
    digitalWrite(2,HIGH);
    carMovement();
  }
  else {
    digitalWrite(2,LOW);
  }
}


void carMovement() {
  // if (Ps3.data.analog.stick.ly<=-25)
  // {
  //     int speed = map(abs(Ps3.data.analog.stick.ly),25,128,0,255);
  //     int speeds[2] = {speed, speed}; // 0 index => left 1 index => right
  //     carMovementX(speeds);
  //     Movement(speeds[0], speeds[1],direction::forward);
  // }
  // else if (Ps3.data.analog.stick.ly>=25)
  // {
  //     int speed = map(abs(Ps3.data.analog.stick.ly),25,128,0,255);
  //     int speeds[2] = {speed, speed}; // 0 index => left 1 index => right
  //     carMovementX(speeds);
  //     Movement(speeds[0], speeds[1],direction::reverse);
  // }
  // else
  // {
  //   Movement(0, 0,direction::stop);
  // }
  // Serial.println(Ps3.data.analog.button.l2);
  if(Ps3.data.analog.button.r2>=25)
  {
    int speed = map(abs(Ps3.data.analog.button.r2),25,150,0,255);
    int speeds[2] = {speed, speed}; // 0 index => left 1 index => right
    carMovementX(speeds);
    Movement(speeds[0], speeds[1],direction::forward);
  }
  else if (Ps3.data.analog.button.l2>=25)
  {
    int speed = map(abs(Ps3.data.analog.button.l2),25,150,0,255);
    int speeds[2] = {speed, speed}; // 0 index => left 1 index => right
    carMovementX(speeds);
    Movement(speeds[0], speeds[1],direction::reverse);
  }
  else
  {
    Movement(0, 0,direction::stop);
  }
}

void carMovementX(int speeds[]) {
  Serial.print("normal ");
  Serial.print(speeds[0]);
  Serial.print(" ");  
  Serial.println(speeds[1]);
  if (Ps3.data.analog.stick.lx<=-25)
  {
      int speedMinus = map(abs(Ps3.data.analog.stick.lx),25,128,0,255);
      speeds[0] -= speedMinus;
  Serial.print("left ");
  Serial.print(speeds[0]);
  Serial.print(" ");  
  Serial.println(speeds[1]);
  }
  else if (Ps3.data.analog.stick.lx>=25)
  {
      int speedMinus = map(abs(Ps3.data.analog.stick.lx),25,128,0,255);
      speeds[1] -= speedMinus;
  Serial.print("right ");
  Serial.print(speeds[0]);
  Serial.print(" ");  
  Serial.println(speeds[1]);
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
  // delay(1);
}


