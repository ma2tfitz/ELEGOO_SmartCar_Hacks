#!/usr/bin/env python3

import sys
import cv2

cap0 = cv2.VideoCapture(0)
cap1 = cv2.VideoCapture(1)
ret0, img0 = cap0.read()
ret1, img1 = cap1.read()

cv2.imshow("live 0", img0)
cv2.imshow("live 1", img1)
cv2.waitKey(4000) & 0xFF == ord('q')

cap0.release()
cap1.release()
