#!/usr/bin/env pybricks-micropython

#|Pybricks import-------------------------------------------------------------------------------------------------|
from pybricks.hubs import EV3Brick
from pybricks.ev3devices import (Motor, TouchSensor, ColorSensor,
                                 InfraredSensor, UltrasonicSensor, GyroSensor)
from pybricks.parameters import Port, Stop, Direction, Button, Color
from pybricks.tools import wait, StopWatch, DataLog
from pybricks.robotics import DriveBase
from pybricks.media.ev3dev import SoundFile, ImageFile
#|----------------------------------------------------------------------------------------------------------------|


from .gyro import *

motorPorts = [Port.A, Port.B, Port.C, Port.D]
sensorPorts = [Port.S1, Port.S2, Port.S3, Port.S4]

class ev3robot:

    def __init__(self, motors, sensors):

        self.controller = "ev3"
        self.brick = EV3Brick()
        self.driveL = ''
        self.driveR = ''
        self.attach1 = ''
        self.attach2 = ''
        self.gyro = ''
        self.colorSensors = []
        self.fillMotors(motors)
        self.fillSensors(sensors)

    def fillMotors(self, motors):
        for i, motor in enumerate(motors):
            if motor[0] == 'driveL':
                self.driveL = Motor(motorPorts[i], positive_direction=getattr(Direction, motor[1]), gears=motor[2])
            elif motor[0] == 'driveR':
                self.driveR = Motor(motorPorts[i], positive_direction=getattr(Direction, motor[1]), gears=motor[2])
            elif motor[0] == 'attach1':
                self.attach1 = Motor(motorPorts[i], positive_direction=getattr(Direction, motor[1]), gears=motor[2])
            elif motor[0] == 'attach2':
                self.attach2 = Motor(motorPorts[i], positive_direction=getattr(Direction, motor[1]), gears=motor[2])

    def fillSensors(self, sensors):
        for i, sensor in enumerate(sensors):
            if isinstance(sensor, gyro):
                self.gyro = sensor
            #elif sensor.name == 'color':
            #    self.colorSensors.append(sensor)
