#!/usr/bin/env pybricks-micropython

from pybricks.parameters import Direction, Port
from pybricks.ev3devices import GyroSensor
from pybricks.iodevices import AnalogSensor, Ev3devSensor
from pybricks.tools import StopWatch
import _thread
import time
# , positive_direction=(Direction.COUNTERCLOCKWISE if reverse else Direction.CLOCKWISE)



class gyro:

    def __init__(self, port, reverse=False):
        self.sensor = Ev3devSensor(getattr(Port, port))
        self.port = getattr(Port, port)
        self.name = 'gyro'
        self.rotation = float(0.0)
        self.rate = ""

    def startGyro(self):
        print('starting gyro...')
        return _thread.start_new_thread(self.gyroTrack())

    def gyroTrack(self):
        print('gyro started')
        oldTime = time.perf_counter()
        while True:
            self.rotation = float(self.rotation)+float((self.sensor.read('GYRO-RATE')[0]*((float(oldTime)-float(time.perf_counter()))/1)))
            oldTime = time.perf_counter()
            self.rate = self.sensor.read('GYRO-RATE')
