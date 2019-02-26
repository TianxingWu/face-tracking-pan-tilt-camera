# -*- coding: utf-8 -*-
"""
Created on Mon Jan 15 18:03:22 2018

@author: James Wu
"""

import cv2
import numpy as np
import serial

face_cascade = cv2.CascadeClassifier('./data/haarcascade_frontalface_default.xml')

#==============================================================================
#   1.多人脸形心检测函数 
#       输入：视频帧
#       输出：各人脸总形心坐标
#==============================================================================
def Detection(frame):
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)  #转换为灰度图
    
    faces = face_cascade.detectMultiScale(gray, 1.3, 5) #人脸检测
    
    if len(faces)>0: 
        for (x,y,w,h) in faces:
            cv2.rectangle(frame, (x,y), (x+w,y+h), (0,255,0), 2)
            X = x+w//2
            Y = y+h//2
            center_pt=(X,Y)   #各人脸中点坐标
            cv2.circle(frame, center_pt, 8, (0,0,255), -1)   #绘制各人脸中点
        centroid_X = int(np.mean(faces, axis=0)[0] + np.mean(faces, axis=0)[2]//2) # 各人脸形心横坐标
        centroid_Y = int(np.mean(faces, axis=0)[1] + np.mean(faces, axis=0)[3]//2) # 各人脸形心纵坐标
        centroid_pt=(centroid_X,centroid_Y)   #各人脸形心坐标
        cv2.circle(frame, centroid_pt, 8, (0,0,255), -1)   #绘制各人脸形心
    else:
     centroid_X = 320
     centroid_Y = 240
    #==========================================================================
    #     绘制参考线
    #==========================================================================
    x = 0;
    y = 0;
    w = 320;
    h = 240;
    
    rectangle_pts = np.array([[x,y],[x+w,y],[x+w,y+h],[x,y+h]], np.int32) #最小包围矩形各顶点
    cv2.polylines(frame, [rectangle_pts], True, (0,255,0), 2) #绘制最小包围矩形
    
    x2 = 320;
    y2 = 240;
    rectangle_pts2 = np.array([[x2,y2],[x2+w,y2],[x2+w,y2+h],[x2,y2+h]], np.int32) #最小包围矩形各顶点
    cv2.polylines(frame, [rectangle_pts2], True, (0,255,0), 2) #绘制最小包围矩形

    #==========================================================================
    #     显示
    #==========================================================================
    cv2.imshow('frame',frame)
    
    return centroid_X,centroid_Y
                
#==============================================================================
#   ****************************主函数入口***********************************
#==============================================================================

# 设置串口参数
ser = serial.Serial()
ser.baudrate = 115200    # 设置比特率为115200bps
ser.port = 'COM3'      # 单片机接在哪个串口，就写哪个串口。这里默认接在"COM3"端口
ser.open()             # 打开串口

#先发送一个中心坐标使初始化时云台保持水平
data = '#'+str('320')+'$'+str('240')+'\r\n'
ser.write(data.encode())        

cap = cv2.VideoCapture(1) #打开摄像头

while(cap.isOpened()):
    _, frame = cap.read()
    
    X, Y = Detection(frame) #执行多人脸形心检测函数
    
    if(X<10000):
        print('X = ')
        print(X)
        print('Y =')
        print(Y)
        #按照协议将形心坐标打包并发送至串口
        data = '#'+str(X)+'$'+str(Y)+'\r\n'
        ser.write(data.encode())
    
    k = cv2.waitKey(5) & 0xFF
    if k==27:   #按“Esc”退出
        break

ser.close()                                     # 关闭串口
cv2.destroyAllWindows()
cap.release()
