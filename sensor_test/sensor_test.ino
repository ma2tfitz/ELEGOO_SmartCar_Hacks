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

#define ECHO_PIN A2
#define TRIG_PIN A3

#define MIN_SPEED 90
#define MAX_SPEED 160

#define STATE_DISABLED 0
#define STATE_SEEKING 1
#define STATE_AVOIDING 2
#define STATE_DRIVING 3

int current_state = STATE_DISABLED;

unsigned char turn_speed = 150;
unsigned char drive_speed = 100;

long rsl_millis = 0;
long rsl_state = LOW;

double current_photo;
double gyro_angle_z_offset;
double gyro_angle_z_target;

long report_millis; // Check Serial interval
long turn_millis; // when did last turn start
int turn_count;
boolean turning_left = true;

// IMU setup
MPU6050 mpu(Wire);

void update_rsl() {
  int timeout = current_state == STATE_DISABLED ? 500 : 2500;
  if (millis() - rsl_millis >= timeout) {
    rsl_millis = millis();
    rsl_state = !rsl_state;
    digitalWrite(LED, rsl_state);
  }
}

// ultrasonic distance
unsigned int get_distance(void) {
  unsigned int duration = 0;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = ((unsigned int)pulseIn(ECHO_PIN, HIGH) / 58); // .0343 cm/uS there & back
  if (duration == 0) {
    return 150; // timeout
  }
  // could clamp to 150cm here
  return duration;
}

double get_photo() {
  double photo_left = analogRead(PHOTO_LEFT);
  double photo_right = analogRead(PHOTO_RIGHT);
  return (photo_left + photo_right) / 2.0;
}

void forward() {
  digitalWrite(ENA,drive_speed);
  digitalWrite(ENB,drive_speed);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void backward() {
  digitalWrite(ENA,drive_speed);
  digitalWrite(ENB,drive_speed);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void left() {
  analogWrite(ENA,turn_speed);
  analogWrite(ENB,turn_speed);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void right() {
  analogWrite(ENA,turn_speed);
  analogWrite(ENB,turn_speed);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void stop(){
  digitalWrite(ENA,LOW);
  digitalWrite(ENB,LOW);
}

void gyro_reset() {
  gyro_angle_z_offset = mpu.getAngleZ();
}

double gyro_get_angle() {
  mpu.update();
  return (mpu.getAngleZ() - gyro_angle_z_offset) / 2.0;
}

double turn_to_light() {
  double max_photo_val = 0;
  double max_photo_angle = 0;
  gyro_reset();
  right();
  while(gyro_get_angle() > -60) {
    delay(50);
  }
  gyro_reset();
  left();
  while(gyro_get_angle() < 120) {
    delay(50);
    double p = get_photo();
    if (p > max_photo_val) {
        max_photo_val = p;
        max_photo_angle = gyro_get_angle();
    }
  }
  right();
  while(gyro_get_angle() > max_photo_angle) {
    delay(50);
  }
  stop();
  return max_photo_angle;
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

  gyro_reset();
  turn_millis = millis();
  turn_count = 0;
}

void loop() {

  mpu.update(); // update frequently
  if (millis() - turn_millis > 5000) {
    turn_millis = millis();
    double p = turn_to_light();
    Serial.println(p);
  }
  
}
