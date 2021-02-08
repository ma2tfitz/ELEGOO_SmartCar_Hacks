/*

 Test updated sensor on the ELEGOO V3+
 - ultrasonic moves to A2/A3 to free I2C pins
 - MPU6050 IMU on I2C bus
 - photoresistors can be read on A0/A1
 - "enabled" LED on pin 13

 The ENA/ENB control speed with PWM
 IN1/IN2 control direction of A
 IN3/IN4 control direction of B

 IN1       IN2       Direction
 ---       ---       ---------
  0         0        Motor OFF
  1         0        Forward
  0         1        Backward
  1         1        Motor OFF

*/

#include "Wire.h"
#include <MPU6050_light.h>

#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

#define LED 13

#define PHOTO_LEFT A0
#define PHOTO_RIGHT A1

#define TRIG_PIN A2
#define ECHO_PIN A3

boolean is_running = false;

unsigned char turn_speed = 180;
unsigned char drive_speed = 180;

long rsl_millis = 0;
long rsl_state = LOW;

// IMU setup
MPU6050 mpu(Wire);
long timer = 0;

void slowturn() {
  is_running = true;
  analogWrite(ENA,90);
  analogWrite(ENB,255);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void forward() {
  is_running = true;
  digitalWrite(ENA,drive_speed);
  digitalWrite(ENB,drive_speed);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void back() {
  is_running = true;
  digitalWrite(ENA,drive_speed);
  digitalWrite(ENB,drive_speed);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void left() {
  is_running = true;
  analogWrite(ENA,turn_speed);
  analogWrite(ENB,turn_speed);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void right() {
  is_running = true;
  analogWrite(ENA,turn_speed);
  analogWrite(ENB,turn_speed);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void stop(){
  is_running = false;
  digitalWrite(ENA,LOW);
  digitalWrite(ENB,LOW);
  digitalWrite(LED, LOW);
  //Serial.println("Stop!");
}

// ultrasonic distance
unsigned int get_distance(void) {
  unsigned int duration = 0;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = ((unsigned int)pulseIn(ECHO_PIN, HIGH, 35) / 58); // .0343 cm/uS there & back
  // could clamp to 150cm here
  return duration;
}

void update_rsl() {
  if (millis() - rsl_millis > 1000) {
    rsl_millis = millis();
    rsl_state = !rsl_state;
    digitalWrite(LED, rsl_state);
  }
}

void setup() {
  Serial.begin(9600);

  // Motor setup
  pinMode(LED,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  stop();

  // Ultrasonic setup
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);

  // IMU setup
  Wire.begin();
 
  byte status = mpu.begin();
  while (status != 0) {
    Serial.print("Unable to connect to MPU6050: ");
    Serial.println(status);
    digitalWrite(LED, HIGH);
    delay(2000);
    digitalWrite(LED, LOW);
    delay(500);
    status = mpu.begin();
  }
  
  digitalWrite(LED, HIGH);
  Serial.print(F("MPU6050 connected: "));
  Serial.println(status);
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true,true); // gyro and accelero
  digitalWrite(LED, LOW);
  Serial.println("MPU6050 Done!\n");
  timer = millis();
}

void loop() {
  update_rsl();
  if (Serial.available()) {
    digitalWrite(LED,HIGH);
    char cmd = Serial.read();
    Serial.print("**** Got: ");
    Serial.print(cmd);
    Serial.print(" ");
    Serial.println((int)cmd);
    switch(cmd) {
      //case 'f': forward(); break;
      //case 'b': back();    break;
    case 'l': left();    break;
    case 't': slowturn();    break;
    case 'r': right();   break;
    case 's': stop();    break;
    default: break;
    }
  }

  mpu.update(); // update frequently

  // log data at most 20 times a second
  // only print if running to avoid filling the buffer
  if (is_running && millis() - timer > 50) {
    double temp = mpu.getTemp();

    double ax = mpu.getAccX();
    double ay = mpu.getAccY();
    double az = mpu.getAccZ();

    double gx = mpu.getGyroX();
    double gy = mpu.getGyroY();
    double gz = mpu.getGyroZ();

    double tx = mpu.getAngleX();
    double ty = mpu.getAngleY();
    double tz = mpu.getAngleZ();

    timer = millis();
   
    double photo_left = analogRead(PHOTO_LEFT);
    double photo_right = analogRead(PHOTO_RIGHT);

    Serial.print(ax);
    Serial.print(" ");
    Serial.print(ay);
    Serial.print(" ");
    Serial.print(az);
    Serial.print(" ");
    Serial.print(gx);
    Serial.print(" ");
    Serial.print(gy);
    Serial.print(" ");
    Serial.print(gz);
    Serial.print(" ");
    Serial.print(tx);
    Serial.print(" ");
    Serial.print(ty);
    Serial.print(" ");
    Serial.print(tz);
    Serial.print(" ");
    Serial.print(temp);
    Serial.print(" ");
    Serial.print(photo_left);
    Serial.print(" ");
    Serial.println(photo_right);
  }

  if (get_distance() < 20) {
    stop();
  }
}
