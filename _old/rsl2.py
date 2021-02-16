#!/usr/bin/env python3
import sys
import serial
import time

serial_speed = 9600
serial_port = "/dev/rfcomm0"
dat = "A"
if __name__ == '__main__':
    ser = serial.Serial(serial_port, serial_speed, timeout=1)
    ser.write(b't')
    for i in range(100):
        dat = ser.readline()
        print(round(time.time()*1000), dat, sep="\t")
    ser.write(b's')
