#include <Servo.h>

// Arduino pin is connected to the IR Receiver
#define RECV_PIN 12

// Arduino pin is connected to the Ultrasonic sensor module
#define ECHO_PIN A2
#define TRIG_PIN A3

// Arduino pin is connected to the Motor drive module
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

// Default LED pin
#define LED_Pin 13

// Arduino pins is connected to the IR tracking module
#define LineTeacking_Pin_Right 10
#define LineTeacking_Pin_Middle 4
#define LineTeacking_Pin_Left 2

// Front servo
#define PIN_Servo 3

Servo servo;             //  Create a DC motor drive object
uint8_t theta = 90;
int photo0;
int photo1;

boolean rsl_status = 0;
unsigned long rsl_previous_ms = 0;

// Turn front servo to clamped angle
void ServoControl(uint8_t angleSetting)
{
  if (angleSetting > 175) {
    angleSetting = 175;
  } else if (angleSetting < 5) {
    angleSetting = 5;
  }
  delays(500);
  // servo.detach();
}

void update_rsl() {
  if (millis() - rsl_previous_ms > 1000) {
    rsl_previous_ms = millis();
    rsl_status = !rsl_status;
    digitalWrite(LED_Pin, rsl_status);
    
  }
}

void setup() {
  Serial.begin(9600);
  servo.attach(PIN_servo);
  theta = 90
  servo.write(theta);
}

void loop() {
  photo0 = analogRead(A0);
  photo1 = analogRead(A1);
  float v0 = photo0 / 1023.0 * 10;
  float v1 = photo1 / 1023.0 * 10  *1.05;
  Serial.print(0);
  Serial.print(" ");
  Serial.print(v0);
  Serial.print(" ");
  Serial.print(v1);
  Serial.print(" ");
  Serial.println(10);
  if (abs(v0 - v1) > 0.1) {
     if (v0 < v1) {
       theta = theta - 1;
     } else {
       theta = theta + 1;
     }
  }
  theta = theta > 175 ? 175 : theta;
  theta = theta < 5 ? 5 : theta;
  servo.write(theta);
  update_rsl();
}
