#include <Arduino.h>
#include <PS4Controller.h>
#include <pid_v1.h>
#include <ESP32Servo.h>

// motor pins
#define IN1 18
#define IN2 5
#define ENA 4
#define IN3 17
#define IN4 16
#define ENB 15

// ir pins
#define IR_LEFTMOST 34
#define IR_LEFT 35
#define IR_CENTER 32
#define IR_RIGHT 33
#define IR_RIGHTMOST 25

//pid
double Setpoint = 0;  // We want to stay centered (position 0)
double Input, Output;
double Kp = 0.6, Ki = 0.01, Kd = 0.2;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

//line follower flag
bool lineFollowerFlag = false;


// Motor speed parameters
int baseSpeed = 150;  // Base speed (0-255)
int maxSpeed = 255;   // Maximum speed (0-255)

//servo
#define SERVO_X_pin 27
#define SERVO_Y1_pin 14
#define SERVO_Y2_pin 12
#define SERVO_Z_pin 13
// servo intial angles
int SERVO_X_ANGLE = 90;
int SERVO_Y1_ANGLE = 90;
int SERVO_Y2_ANGLE = 90;
int SERVO_Z_ANGLE = 90;

// servo objects
Servo SERVO_X;
Servo SERVO_Y1;
Servo SERVO_Y2;
Servo SERVO_Z;


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
void readSensors();
void PIDStart();
void calculatePID();
void motorControl();

void notify()
{
  if(PS4.event.button_down.circle)
  {
    // Serial.println("Circle Pressed");
    lineFollowerFlag = !lineFollowerFlag;
    Serial.println(lineFollowerFlag);
    
  }
}
void setup() {
  // Motors 
  pinMode(2, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(IR_LEFTMOST, INPUT);
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_CENTER, INPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(IR_RIGHTMOST, INPUT);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  // PID INIT
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-maxSpeed, maxSpeed);

  //ps4 init
  PS4.attach(notify);
  PS4.begin("1a:2b:3c:01:01:01");


  // servo motor 
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
    if(lineFollowerFlag)
    {
      // Serial.println("Line Follower Mode");
      PS4.setLed(255,0,0);
      PIDStart();
    }
    else
    {
    // Serial.println("PS4 Controller Mode");
    digitalWrite(2,HIGH);
    carMovement();
    // armAngles();
    }
  }
  else {
    digitalWrite(2,LOW);
    PIDStart();

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
  // Serial.println("Right");
  int speedMinus = map(abs(PS4.data.analog.stick.lx),5,128,0,speeds[1]);
  speeds[1] -= speedMinus; 
  Serial.println(speeds[1]);
  }
  else if (PS4.data.analog.stick.lx<=-5)
  {
  int speedMinus = map(abs(PS4.data.analog.stick.lx),5,128,0,speeds[0]);
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
      // delay(500);
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



// ====================================================================
// ====================================================================
// ====================================================================

// PID parameters




void readSensors() {
  // Read sensor values and calculate position
  // Line is black (0), background is white (1)
  int leftmost = !digitalRead(IR_LEFTMOST);  // Invert if needed
  int left = !digitalRead(IR_LEFT);
  int center = !digitalRead(IR_CENTER);
  int right = !digitalRead(IR_RIGHT);
  int rightmost = !digitalRead(IR_RIGHTMOST);
  Serial.print(leftmost);
  Serial.print(" ");
  Serial.print(left);
  Serial.print(" ");
  Serial.print(center);
  Serial.print(" ");
  Serial.print(right);
  Serial.print(" ");
  Serial.println(rightmost);

  // Calculate position (-4 to +4)
  if (leftmost && !left && !center && !right && !rightmost) Input = -4;
  else if (leftmost && left && !center && !right && !rightmost) Input = -3;
  else if (!leftmost && left && !center && !right && !rightmost) Input = -2;
  else if (!leftmost && left && center && !right && !rightmost) Input = -1;
  else if (!leftmost && !left && center && !right && !rightmost) Input = 0;
  else if (!leftmost && !left && center && right && !rightmost) Input = 1;
  else if (!leftmost && !left && !center && right && !rightmost) Input = 2;
  else if (!leftmost && !left && !center && right && rightmost) Input = 3;
  else if (!leftmost && !left && !center && !right && rightmost) Input = 4;
  else if (leftmost && left && center && right && rightmost) Input = 0;  // All sensors on line
  else Input = 0;  // No line detected or other cases
  Serial.println(Input);
}

void calculatePID() {
  myPID.Compute();
}

void motorControl() {
  int leftMotorSpeed, rightMotorSpeed;
  
  // Calculate motor speeds based on PID output
  leftMotorSpeed = baseSpeed - Output;
  rightMotorSpeed = baseSpeed + Output;
  
  // Constrain speeds to 0-maxSpeed
  leftMotorSpeed = constrain(leftMotorSpeed, 0, maxSpeed);
  rightMotorSpeed = constrain(rightMotorSpeed, 0, maxSpeed);
  Serial.println("Left:");
  Serial.println(leftMotorSpeed);
  Serial.println("Right:");
  Serial.println(rightMotorSpeed);
  delay(1000);
  // Movement(leftMotorSpeed, rightMotorSpeed, direction::forward);
}

void PIDStart()
{
  readSensors();  // Read sensor values
  calculatePID();  // Calculate PID output
  motorControl();  // Control motors based on PID output
}