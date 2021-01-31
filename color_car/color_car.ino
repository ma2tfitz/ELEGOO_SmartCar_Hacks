#include <Servo.h>  //servo library

#include "DEV_Config.h"
#include "TCS34725.h"

RGB rgb,RGB888;
UWORD lux;
int distance;

Servo myservo;      // create servo object to control servo

int Echo = A2;  
int Trig = A3; 

#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11
#define LED 13

unsigned char carSpeed = 100;
bool state = LOW;
int rightDistance = 0, leftDistance = 0, middleDistance = 0;

void forward(){
  digitalWrite(ENA,carSpeed);
  digitalWrite(ENB,carSpeed);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  Serial.println("Forward");
}

void back(){
  digitalWrite(ENA,carSpeed);
  digitalWrite(ENB,carSpeed);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  Serial.println("Back");
}

void left(){
  analogWrite(ENA,carSpeed);
  analogWrite(ENB,carSpeed);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  Serial.println("Left");
}

void right(){
  analogWrite(ENA,carSpeed);
  analogWrite(ENB,carSpeed);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  Serial.println("Right");
}

void stop(){
  digitalWrite(ENA,LOW);
  digitalWrite(ENB,LOW);
  Serial.println("Stop!");
}

void stateChange(){
  state = !state;
  digitalWrite(LED, state);
  //Serial.println("Light");  
}

//Ultrasonic distance measurement Sub function
int Distance_test() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);   
  float Fdistance = pulseIn(Echo, HIGH);  
  Fdistance= Fdistance / 58;       
  return (int)Fdistance;
}  

void setup() {
  Serial.begin(9600);
  
  Wire.begin(); // i2c
  myservo.attach(3,700,2400);  // attach servo on pin 3 to servo object
  pinMode(LED, OUTPUT);
  pinMode(Echo, INPUT);    
  pinMode(Trig, OUTPUT);  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  stop();

  if(TCS34725_Init() != 0){
      Serial.print("TCS34725 initialization error!!\r\n");
  } else {
    Serial.print("TCS34725 initialization success!!\r\n");
  }

}

void loop() {

  distance = Distance_test();
  if (distance < 40) {
    state = HIGH;
  } else {
    state = LOW;
  }
  digitalWrite(LED, state);    

  if (Serial.available()) {
    char getstr = Serial.read();
    switch(getstr) {
	    case 'f': forward(); break;
	    case 'b': back();   break;
	    case 'l': left();   break;
	    case 'r': right();  break;
	    case 's': stop();   break;
	    case '<': myservo.write(10); break;
	    case '|': myservo.write(90); break;
	    case '>': myservo.write(170); break;
	    default:  break;
    }
  }
  
  if (distance <= 40) {
    stop();
  }
  
  rgb=TCS34725_Get_RGBData();
  lux =TCS34725_Get_Lux(rgb);
  //RGB888=TCS34725_GetRGB888(rgb);
  Serial.print(lux);
  Serial.print(",");
  Serial.print(rgb.R);
  Serial.print(",");
  Serial.print(rgb.G);
  Serial.print(",");
  Serial.print(rgb.B);
  Serial.print(",");
  Serial.print(rgb.C);
  Serial.print(",");
  Serial.print(distance);
  Serial.print("\r\n");
  DEV_Delay_ms(100);
}

// -------------------------------------------------------------------------------

void avoid() { 
    myservo.write(90);  //setservo position according to scaled value
    delay(500); 
    middleDistance = Distance_test();

    if(middleDistance <= 40) {     
      stop();
      delay(500);                         
      myservo.write(10);          
      delay(1000);      
      rightDistance = Distance_test();
      
      delay(500);
      myservo.write(90);              
      delay(1000);                                                  
      myservo.write(180);              
      delay(1000); 
      leftDistance = Distance_test();
      
      delay(500);
      myservo.write(90);              
      delay(1000);
      if(rightDistance > leftDistance) {
        right();
        delay(360);
      }
      else if(rightDistance < leftDistance) {
        left();
        delay(360);
      }
      else if((rightDistance <= 40) || (leftDistance <= 40)) {
        back();
        delay(180);
      }
      else {
        forward();
      }
    }  
    else {
        forward();
    }                     
}
