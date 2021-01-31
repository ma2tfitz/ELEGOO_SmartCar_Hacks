# ELEGOO_SmartCar_Hacks

The ELEGOO Smart Robot Car Kit V3.0+ is a simple bluetooth enabled Arduino UNO R3 based robot car kit.

https://www.amazon.com/ELEGOO-Tracking-Ultrasonic-Intelligent-Educational/dp/B07KPZ8RSZ/ref=sr_1_2

We are looking at ways to modify the kit by adding other sensors and behaviors. By default, the ultrasonic sensort is hooked to the same Arduino pins (A4 & A5) that would be used for I2C. By moving these to A2 & A3, we're able to add other I2C devices such as simple 6-axis IMU, color sensors, etc. We can also add aditional sensors to the open analog pins A0 & A1, such as photoresistors, a GP2Y0A21 IR distance sensor, etc.

ELEGOO technical support confirmed that the included Bluetooth module works only with their iPhone apps. Adding a different $8 HC-05 (we used HiLetgo) allows the car to send and receive data for more complex computer-based control.

The Default_Main was unpacked from "ELEGOO Smart Robot Car Kit V3.0 Plus2020.12.14.zip" downloaded from:
https://www.elegoo.com/pages/arduino-kits-support-files
https://drive.google.com/file/d/10RocvdFENY07yfJZKfnbr3WpP7TLhegN/view


