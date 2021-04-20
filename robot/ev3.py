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
        return DriveBase(self.driveL, self.driveR, dimentions.diameter, dimentions.axleLength)

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
    def __init__(robot, diameter, axleLength, fullRotation=360):
        self.diameter = diameter
        self.axleLength = axleLength
        self.assignedRobot = robot
        self.lengthModifier = fullRotation/(3.14159265358979 * diameter) #calculates the value we need to muliply distance by to get target degrees
        self.driveSpeed = 50
        self.driveAcceleration = 50
        self.turnSpeed = 10
        self.turnAcceleration = 50

    def tankControl(self, speed=50, steering=0):
        if speed != 0:
            self.assignedRobot.driveL.run(speed+steering)
            self.assignedRobot.driveR.run(speed-steering)
        else:
            self.assignedRobot.driveL.hold()
            self.assignedRobot.driveR.hold()

        
    def drive(self, length, speed, rotation='use start'):
        if rotation == 'use start':
            rotation = self.assignedRobot.gyro.rotation

        driving = True
        startTime = time.perf_counter()
        while driving:
            if time.perf_counter() < self.driveSpeed/self.driveAcceleration:
                self.tankControl(speed=(time.perf_counter()/(self.driveSpeed/self.driveAcceleration)*self.driveSpeed))
#---------------------------------------------------------------------------------------------------------------------------------------------------|
