#!/usr/bin/env python3
import sys
import serial
import time

serial_speed = 9600
serial_port = "/dev/rfcomm0"
dat = "A"
commands = [b't', b'w', b'w', b'v', b'v', b'u', b'u', b'u', b't', b't', b't']
commands = [b't', b'u', b'v', b'w', b'x',][::-1]
if __name__ == '__main__':
    ser = serial.Serial(serial_port, serial_speed, timeout=1)
    time.sleep(5)
    #ser.write(b'e')
    start_time = time.time()
    while True:
        t = time.time()
        dat = ser.readline()
        if dat: print(t, dat)
        if t - start_time > 30:
            break
    #ser.write(b'd')
    start_time = time.time()
    while True:
        t = time.time()
        dat = ser.readline()
        if dat: print(t, dat)
        if t - start_time > 30:
            break
