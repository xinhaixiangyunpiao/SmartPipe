import sys
sys.path.append('/home/lx/SmartPipe/models/yolo/')
sys.path.append('/home/lx/SmartPipe/models/yolo/repo/yolov5/')

from yolo import yolo
a = yolo()
a.load_model()

import cv2
import numpy as np

cap = cv2.VideoCapture("/home/lx/SmartPipe/data_source/videos/road.webm")
ret, frame = cap.read()

frame = cv2.resize(frame, (1920,1080), interpolation = cv2.INTER_AREA)

img_RGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

x = np.array(img_RGB)

a.inference([x])
