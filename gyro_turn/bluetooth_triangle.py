#!/usr/bin/env python3
import sys
import serial
import time

serial_speed = 9600
serial_port = "/dev/rfcomm0"
dat = "A"

if __name__ == '__main__':
    ser = serial.Serial(serial_port, serial_speed, timeout=1)

    while True:
        dat = ser.readline()
        dat = dat.decode("utf8", "ignore")
        fields = dat.rstrip().split(" ")
        print(*fields, sep="\t")
        if len(fields) < 3:
            break

    print("waiting")
    time.sleep(5)

    for i in range(3):
        ser.write(b'f')
        time.sleep(0.4)
        while True:
            dat = ser.readline()
            dat = dat.decode("utf8", "ignore")
            fields = dat.rstrip().split(" ")
            print(*fields, sep="\t")
            if len(fields) < 3:
                break
        ser.write(b'x')
        while True:
            dat = ser.readline()
            dat = dat.decode("utf8", "ignore")
            fields = dat.rstrip().split(" ")
            print(*fields, sep="\t")
            if len(fields) < 3:
                break

    ser.write(b's')
