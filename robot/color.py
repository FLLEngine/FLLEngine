#!/usr/bin/env pybricks-micropython

from pybricks.iodevices import Ev3devSensor
from pybricks.parameters import Port

class color:
    def __init__(self, port):
        self.sensor = Ev3devSensor(getattr(Port, port))
        self.port = getattr(Port, port)
        self.name = 'color'

    def read(self):
        return self.sensor.read("RGB-RAW")
