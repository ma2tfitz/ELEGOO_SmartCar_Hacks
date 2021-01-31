#include <Servo.h>

#define PIN_Servo 3
Servo servo;

// Default LED pin
#define LED_Pin 13

boolean rsl_status = LOW;
unsigned long rsl_previous_ms = 0;

int theta;

int photo0;
int photo1;

void update_rsl() {
  if (millis() - rsl_previous_ms > 1000) {
    rsl_previous_ms = millis();
    if (rsl_status == LOW) {
      rsl_status = HIGH;
    } else {
      rsl_status = LOW;
    }
    digitalWrite(LED_Pin, rsl_status);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_Pin, OUTPUT);
  servo.attach(PIN_Servo);
  theta = 90;
  servo.write(theta);
  //pinMode(A0, INPUT);
  //pinMode(A1, INPUT);
}

void loop() {
  photo0 = analogRead(A0);
  photo1 = analogRead(A1) * 1.7;
  float v0 = photo0;
  float v1 = photo1;
  Serial.print(0);
  Serial.print(" ");
  Serial.print(v0);
  Serial.print(" ");
  Serial.print(v1);
  Serial.print(" ");
  Serial.println(10);

  if (abs(v1 - v0) > 12) {
    if (v0 > v1) {
      theta = theta - 1;
      theta = theta < 5 ? 5 : theta;
    } else {
      theta = theta + 2;
      theta = theta > 175 ? 175 : theta;
    }
    servo.write(theta);
  }
  delay(100);
  update_rsl();
}
