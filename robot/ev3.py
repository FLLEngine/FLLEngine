#!/usr/bin/env pybricks-micropython
# -*- coding: utf-8 -*-

from pybricks.hubs import EV3Brick
from pybricks.ev3devices import *

from pybricks.parameters import *
from pybricks.tools import *
from pybricks.robotics import *
from pybricks.media.ev3dev import *

motorPorts = [Port.A, Port.B, Port.C, Port.D]
sensorPorts = [Port.S1, Port.S2, Port.S3, Port.S4]

class ev3robot:

    def __init__(self):

        self.controller = "ev3"
        self.brick = EV3Brick()
        self.driveL = ''
        self.driveR = ''
        self.attach1 = ''
        self.attach2 = ''
        self.gyro = ''
        self.colorSensors = []

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
            else:
                print('empty port recognized')

    def fillSensors(self, sensors):
        for i, sensor in enumerate(sensors):
            if sensor[0] == 'gyro':
                self.gyro = GyroSensor(sensorPorts[i], positive_direction=getattr(Direction, sensor[1]))
            if sensor == 'color':
                self.colorSensors.append(ColorSensor(sensorPorts[i]))
