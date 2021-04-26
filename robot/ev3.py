#!/usr/bin/env pybricks-micropython

#|Pybricks import-------------------------------------------------------------------------------------------------|
from pybricks.hubs import EV3Brick
from pybricks.ev3devices import (Motor, TouchSensor, ColorSensor,
                                 InfraredSensor, UltrasonicSensor, GyroSensor)
from pybricks.parameters import Port, Stop, Direction, Button, Color
from pybricks.tools import wait, StopWatch, DataLog
from pybricks.robotics import DriveBase
from pybricks.media.ev3dev import SoundFile, ImageFile
from pybricks.iodevices import Ev3devSensor
#|----------------------------------------------------------------------------------------------------------------|

import time
from math import cos, sin
import _thread

motorPorts = [Port.A, Port.B, Port.C, Port.D]
sensorPorts = [Port.S1, Port.S2, Port.S3, Port.S4]



#--general-robot-stuff------------------------------------------------------------------------------------------------------------------------------|
class ev3robot:

    def __init__(self, motors, sensors, dimentions):

        self.controller = "ev3"
        self.brick = EV3Brick()
        self.driveL = ''
        self.driveR = ''
        self.attach1 = ''
        self.attach2 = ''
        self.gyro = ''
        self.location = (0,0)
        self.colorSensors = []
        self._fillMotors(motors)
        self._fillSensors(sensors)
        self.train = self._createTrain(dimentions)

    def _fillMotors(self, motors):
        for i, motor in enumerate(motors):
            if motor[0] == 'driveL':
                self.driveL = Motor(motorPorts[i], positive_direction=getattr(Direction, motor[1].upper()), gears=motor[2])
            elif motor[0] == 'driveR':
                self.driveR = Motor(motorPorts[i], positive_direction=getattr(Direction, motor[1].upper()), gears=motor[2])
            elif motor[0] == 'attach1':
                self.attach1 = Motor(motorPorts[i], positive_direction=getattr(Direction, motor[1].upper()), gears=motor[2])
            elif motor[0] == 'attach2':
                self.attach2 = Motor(motorPorts[i], positive_direction=getattr(Direction, motor[1].upper()), gears=motor[2])

    def _fillSensors(self, sensors):
        for i, sensor in enumerate(sensors):
            if isinstance(sensor, gyro):
                self.gyro = sensor
            elif isinstance(sensor, color):
                self.colorSensors.append(sensor)

    def _createTrain(self, dimentions):
        return driveBase(self, dimentions.diameter, dimentions.axleLength)

    def _MPS(self):
        print('MPS started')
        previousAngle = self.gyro.rotation
        previousWheelPos =  (self.driveL.angle(), self.driveR.angle())
        oldTime = time.perf_counter()
        while True:
            angle = (self.gyro.rotation+previousAngle)/2
            wheelPos = (self.driveL.angle(), self.driveR.angle())
            forward = ((wheelPos[0]-previousWheelPos[0]) + (wheelPos[1]-previousWheelPos[1]))/2
            #elaborate if statements
            x = ((angle<90)*(forward*cos(angle))  +  ((angle>=90 and angle<180)*(0-(forward*cos(90-(angle-90)))))  +  ((angle>-180 and angle<270)*(0-(forward*cos(angle-180))))  +  ((angle>=270 and angle<360)*(forward*cos(90-(angle-270)))))
            y = ((angle<90)*(forward*sin(angle))  +  ((angle>=90 and angle<180)*(forward*sin(90-(angle-90))))  +  ((angle>-180 and angle<270)*(0-(forward*sin(angle-180))))  +  ((angle>=270 and angle<360)*(0-(forward*sin(90-(angle-270))))))
            self.location = (self.location[0]+x, self.location[1]+y)
            print(self.location, angle)
            previousAngle = angle
            previousWheelPos = wheelPos

    def waitForButton(self, selectedButton):
        waiting = True
        while waiting:
            for i, button in enumerate(self.brick.buttons.pressed()):
                if button == getattr(Button, str(selectedButton.upper())):
                    waiting = False

    def MPSStart(self):
        self.driveL.reset_angle(0)
        self.driveR.reset_angle(0)
        if self.gyro.running == False:
            self.gyro.startGyro()

        _thread.start_new_thread(self._MPS, ())
#---------------------------------------------------------------------------------------------------------------------------------------------------|




#--color-sensor-stuff-------------------------------------------------------------------------------------------------------------------------------|
class color:
    def __init__(self, port):
        self.sensor = Ev3devSensor(getattr(Port, port.upper()))
        self.port = getattr(Port, port.upper())
        self.name = 'color'

    def read(self):
        return self.sensor.read("RGB-RAW")
#---------------------------------------------------------------------------------------------------------------------------------------------------|




#--gyro-stuff---------------------------------------------------------------------------------------------------------------------------------------|
class gyro:

    def __init__(self, port, reverse=False):
        self.sensor = Ev3devSensor(getattr(Port, port.upper()))
        self.port = getattr(Port, port.upper())
        self.name = 'gyro'
        self.rotation = float(0.0)
        self.rate = ""
        self.running = False

    def startGyro(self):
        print('starting gyro...')
        self.running = True
        return _thread.start_new_thread(self._gyroTrack, ())

    def _gyroTrack(self):
        print('gyro started')
        oldTime = time.perf_counter()
        while True:
            self.rotation = float(self.rotation)+float((self.sensor.read('GYRO-RATE')[0]*((float(oldTime)-float(time.perf_counter()))/1)))
            oldTime = time.perf_counter()
            self.rate = self.sensor.read('GYRO-RATE')
#---------------------------------------------------------------------------------------------------------------------------------------------------|




#--custom-drive-base--------------------------------------------------------------------------------------------------------------------------------|
class driveBase:
    def __init__(self, robot, diameter, axleLength, fullRotation=360):
        self.diameter = diameter
        self.axleLength = axleLength
        self.assignedRobot = robot
        self.lengthModifier = fullRotation/(3.14159265358979 * diameter) #calculates the value we need to muliply distance by to get target degrees
        self.rotToMM = (3.14159265358979*diameter/fullRotation)
        self.driveSpeed = 1000
        self.driveAcceleration = 500
        self.turnSpeed = 10
        self.turnAcceleration = 50
        self.decelerationWall = 50
        self.straightSensitivity = 1

    def tankDistance(self, subtract=(0,0)):
        return abs((((self.assignedRobot.driveL.angle()-subtract[0]) + (self.assignedRobot.driveR.angle()-subtract[1]))/2)*self.rotToMM)

    def tankControl(self, speed=50, steering=0):
        if speed != 0:
            self.assignedRobot.driveL.run(speed+steering)
            self.assignedRobot.driveR.run(speed-steering)
        else:
            self.assignedRobot.driveL.hold()
            self.assignedRobot.driveR.hold()


    def drive(self, length, useGyro=True, rotation='use start'):
        if rotation == 'use start':
            rotation = self.assignedRobot.gyro.rotation

        driving = True
        startPositions = (self.assignedRobot.driveL.angle(), self.assignedRobot.driveR.angle())
        startTime = time.perf_counter()
        vel = 0
        neg = False
        startSpeed = (self.assignedRobot.driveL.speed()+self.assignedRobot.driveR.speed())/2
        if length < 0:
            length = 0-length
            neg = True

        while vel<self.driveSpeed and self.tankDistance(subtract=startPositions)<length/2:
            vel = ((time.perf_counter()-startTime)/((self.driveSpeed-startSpeed)/self.driveAcceleration)*(self.driveSpeed-startSpeed))
            self.tankControl(speed=((neg==True) * (0-vel) + (neg==False) * vel), steering=(self.assignedRobot.driveR.angle()-self.assignedRobot.driveL.angle())*self.straightSensitivity)

        accelTime = time.perf_counter()-startTime
        accelDistance = self.tankDistance(subtract=startPositions)
        while self.tankDistance(subtract=startPositions)<length-accelDistance:
            vel = self.driveSpeed
            self.tankControl(speed=((neg==True) * (0-vel) + (neg==False) * vel), steering=(self.assignedRobot.driveR.angle()-self.assignedRobot.driveL.angle())*self.straightSensitivity)

        print(startTime)
        startTime = time.perf_counter()
        decelSpeed = vel
        print(accelDistance, decelSpeed, startTime)
        while self.tankDistance(subtract=startPositions)<length:
            vel = max(decelSpeed-(time.perf_counter()-startTime)/(accelTime*0.85)*decelSpeed, self.decelerationWall)
            self.tankControl(speed=((neg==True) * (0-vel) + (neg==False) * vel), steering=(self.assignedRobot.driveR.angle()-self.assignedRobot.driveL.angle())*self.straightSensitivity)

        self.tankControl(speed=0)
        print('stopped')
        print(self.tankDistance(subtract=startPositions))

#---------------------------------------------------------------------------------------------------------------------------------------------------|
