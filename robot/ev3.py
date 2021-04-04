#!/usr/bin/env python
# -*- coding: utf-8 -*-

from pybricks.hubs import EV3Brick
from pybricks.ev3devoces import *

from pybricks.parameters import *
from pybricks.tools import *
from pybricks.robotics import *
from pybricks.media.ev3dev import *



class ev3robot:

    def __init__(self, driveL='A', driveR='B', attach1='C', attach2='D', S1='empty', S2='empty', S3='empty', S4='empty'):

        self.controller = "ev3"
        self.brick = EV3Brick()
        self.brick.driveL = Motor(getattr(Port, driveL))
        self.brick.driveR = Motor(getattr(Port, driveR))
        self.brick.attach1 = Motor(getattr(Port, attach1))
        self.brick.attach2 = Motor(getattr(Port, attach2))
        for i, item in enumerate([S1, S2, S3, S4]):
            if i[0] =='gyro':
                self.gyro = GyroSensor(getattr(Port, 'S'+(i+1)), getattr(Direction, i[1]))
            elif i[0] == 'color':
                self.colorSensors[i[1]] = ColorSensor(getattr(Port, 'S'+(i+1)))



