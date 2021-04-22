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

    def waitForButton(self, selectedButton):
        waiting = True
        while waiting:
            for i, button in enumerate(self.brick.buttons.pressed()):
                if button == getattr(Button, str(selectedButton.upper())):
                    waiting = False
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

    def startGyro(self):
        print('starting gyro...')
        return _thread.start_new_thread(self.gyroTrack, ())

    def gyroTrack(self):
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
        self.driveSpeed = 1500
        self.driveAcceleration = 500
        self.turnSpeed = 10
        self.turnAcceleration = 50
        self.decelerationWall = 10
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
        accelDistance = 0
        startedDecel = False
        first = True
        firstDecel = True
        startTime = time.perf_counter()
        vel = 0
        neg = False
        manSwitch = False
        if length < 0:
            length = 0-length
            neg = True

        while (self.tankDistance(subtract=startPositions)<=length):
            if self.tankDistance(subtract=startPositions)<length-accelDistance and manSwitch==False:
                if first:
                    startTime = time.perf_counter()
                    first = False

                if vel < self.driveSpeed:
                    vel = ((time.perf_counter()-startTime)/(self.driveSpeed/self.driveAcceleration)*self.driveSpeed)
                    accelDistanceI = self.tankDistance(subtract=startPositions)
                    stage = 'accel'
                    if self.tankDistance(subtract=startPositions)>=length/2:
                        manSwitch = True
                        accelDistance = accelDistanceI-5
                        decelSpeed = vel
                        startDecel=time.perf_counter()

                else:
                    vel = self.driveSpeed
                    accelDistance = accelDistanceI
                    decelSpeed = vel
                    startDecel=time.perf_counter()
                    stage = 'drive'

            elif self.tankDistance(subtract=startPositions)<length:
                stage = 'decel'
                if decelSpeed-((time.perf_counter()-startDecel)/(decelSpeed/self.driveAcceleration)*decelSpeed) > self.decelerationWall:
                    print((self.driveSpeed-(time.perf_counter()-startDecel)/(self.driveSpeed/self.driveAcceleration)*self.driveSpeed)-(self.driveSpeed-decelSpeed), (self.driveSpeed-(time.perf_counter()-startDecel)/(self.driveSpeed/self.driveAcceleration)*self.driveSpeed), (self.driveSpeed-decelSpeed), decelSpeed)
                    if manSwitch:
                        vel = (self.driveSpeed-(time.perf_counter()-startDecel)/(self.driveSpeed/self.driveAcceleration)*self.driveSpeed)-(self.driveSpeed-decelSpeed)# decelSpeed-(time.perf_counter()-startDecel)/(startDecel-startTime)*decelSpeed-(self.driveSpeed-decelSpeed)

                    else:
                        vel = self.driveSpeed-(time.perf_counter()-startDecel)/(self.driveSpeed/self.driveAcceleration)*self.driveSpeed

                else:
                    vel = self.decelerationWall
                    print('decwall')

            #got that branchless if statement in there :D
            self.tankControl(speed=((neg==True) * (0-vel) + (neg==False) * vel), steering=(self.assignedRobot.driveR.angle()-self.assignedRobot.driveL.angle())*self.straightSensitivity)
            print(stage, self.tankDistance(subtract=startPositions), vel, length)

        self.tankControl(speed=0)
        print('stopped')
        print(self.tankDistance(subtract=startPositions))

#---------------------------------------------------------------------------------------------------------------------------------------------------|
