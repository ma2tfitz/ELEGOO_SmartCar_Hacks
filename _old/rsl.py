#!/usr/bin/env python3
import sys
import serial
import time

serial_speed = 9600
serial_port = "/dev/rfcomm9"
dat = "A"
if __name__ == '__main__':
    ser = serial.Serial(serial_port, serial_speed, timeout=1)
    for i in range(5):
        ser.write(b'1')
        dat = ser.readline()
        print(dat)
        time.sleep(0.1)
        ser.write(b'0')
        dat = ser.readline()
        print(dat)
        time.sleep(0.1)
