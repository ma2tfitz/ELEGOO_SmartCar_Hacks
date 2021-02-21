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

unsigned char turn_speed = 120;
unsigned char drive_speed = 100;

long rsl_millis = 0;
long rsl_state = LOW;

double current_photo;
double gyro_angle_z_offset;
double gyro_angle_z_target;

// IMU setup
MPU6050 mpu(Wire);

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
  is_running = false;

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
  gyro_reset();
  Serial.println("MPU6050 Done!\n");
}


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

void enable() {
  current_state = STATE_SEEKING;
  stop();
}

void disable() {
  current_state = STATE_DISABLED;
  stop();
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

void backward() {
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

void turn_to_angle(double angle) {
  gyro_angle_z_target = angle;
  gyro_reset();
  if (angle > 0) {
    left();
  } else {
    right();
  }
  start_millis = millis();
  end_millis = start_millis + 5000; // give up in 5 seconds
}

boolean is_turn_done() {
  if (millis() > end_millis) {
    return true; // give up on timeout
  }
  double delta = gyro_angle_z_target - gyro_get_angle();
  if (delta > 0) {
    left();
  } else {
    right();
  }
  return abs(delta) < 0.35;
}

void turn_to_light() {
  double photo_max_angle = 0;
  double photo_max_value = 0;
  gyro_reset();
  left();
  while(gyro_get_angle() < 45) {
  }
  stop();
  gyro_reset();
  right();
  while(gyro_get_angle() > -45) {
    double p = get_photo();
    if (p > photo_max_value) {
      photo_max_value = p;
      photo_max_angle = gyro_get_angle();
    }
  }
  stop();
  left();
  while(gyro_get_angle() < photo_max_angle) {
  }
  stop();
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    Serial.print("**** Got: ");
    Serial.println(cmd);
    switch(cmd) {
    case 'e': enable();  break; // go
    case 'd': disable();  break;
    default: break;
    }
  }

  update_rsl();
  mpu.update(); // update frequently

  int distance = get_distance();
  if (distance < 20) {
    backward();
    delay(100);
    stop();
    current_state = STATE_AVOIDING;
    turn_to_angle(180);
  }
  if (current_state == STATE_AVOIDING) {
    if (is_turn_done()) {
      stop();
      current_state = STATE_SEEKING;
    }
  } else if (current_state == STATE_SEEKING) {
    turn_to_light();
    current_photo = get_photo();
    current_state = STATE_DRIVING;
  } else if (current_state == STATE_DRIVING) {
    forward();
    if (get_photo < current_photo * 0.9) {
      stop();
      current_state = STATE_SEEKING;
    }
  }
}
