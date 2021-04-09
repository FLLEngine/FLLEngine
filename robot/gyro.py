#!/usr/bin/env pybricks-micropython

from pybricks.parameters import Direction
from pybricks.ev3devices import GyroSensor
import _thread

class gyro:

    def __init__(self, port, reverse=False):
        self.basic = GyroSensor(port, positive_direction=(Direction.COUNTERCLOCKWISE if reverse else Direction.CLOCKWISE))
        self.name = 'gyro'
        self.rotation = ""
        self.rate = ""

    def startGyro(self):
        print('starting gyro...')
