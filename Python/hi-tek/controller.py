import pygame
from threading import Thread
import time
import serial
import numpy as np

use_serial = True
use_controller = True
invertible = True

lowrange = True

firsttime = True

last_received = ''

COMport = 'com18'


def receiving(serial_port):
    global last_received
    buffer = ''
    while True:
        buffer += serial_port.read_all()
        if '\n' in buffer:
            lines = buffer.split('\n')  # Guaranteed to have at least 2 entries
            last_received = lines[-2]
            # If the Arduino sends lots of empty lines, you'll lose the last
            # filled line, so you could make the above statement conditional
            # like so: if lines[-2]: last_received = lines[-2]
            buffer = lines[-1]
            print last_received


class SerialData(object):
    def __init__(self):
        try:
            self.serial_port = serial.Serial(COMport, 2400)
        except serial.serialutil.SerialException:
            # no serial connection
            self.serial_port = None
        else:
            Thread(target=receiving, args=(self.serial_port,)).start()

    def send(self, data):
        self.serial_port.write(data)

    def __del__(self):
        if self.serial_port is not None:
            self.serial_port.close()


leftspeed = 0
rightspeed = 0
prerpressed = False
rpressed = False
reversed = False


def getkey():
    global leftspeed
    global rightspeed
    global prerpressed
    global rpressed
    global reversed
    leftspeed = 0
    rightspeed = 0
    wpressed = False
    spressed = False
    apressed = False
    dpressed = False
    prerpressed = rpressed
    rpressed = False
    press = pygame.key.get_pressed()
    for i in xrange(0, len(press)):
        if press[i] == 1:
            name = pygame.key.name(i)
            if(name == "w"):
                wpressed = True
            elif(name == "s"):
                spressed = True
            elif(name == "a"):
                apressed = True
            elif(name == "d"):
                dpressed = True
            elif(name == "r"):
                rpressed = True
    if(wpressed):
        leftspeed = leftspeed + 100
        rightspeed = rightspeed + 100
    if(spressed):
        leftspeed = leftspeed - 100
        rightspeed = rightspeed - 100
    if(apressed):
        leftspeed = leftspeed - 100
        rightspeed = rightspeed + 100
    if(dpressed):
        leftspeed = leftspeed + 100
        rightspeed = rightspeed - 100
    if (prerpressed and (not rpressed)):
        reversed = not reversed
    if(leftspeed > 100):
        leftspeed = 100
    if (leftspeed < -100):
        leftspeed = -100
    if (rightspeed > 100):
        rightspeed = 100
    if (rightspeed < -100):
        rightspeed = -100
    if(reversed):
        c = leftspeed
        leftspeed = -rightspeed
        rightspeed = -c


if __name__ == '__main__':
    if use_serial:
        s = SerialData()

    pygame.init()  # Initialize the PyGame package
    pygame.display.set_mode((1000, 600))  # Make the window 1000*600
    pygame.display.set_caption('Testing')  # Make the caption "Testing"
    if use_controller:
        pygame.joystick.init()
        joysticks = [pygame.joystick.Joystick(
            x) for x in range(pygame.joystick.get_count())]
        for joystick in joysticks:
            joystick.init()
            print joystick.get_name
    while True:
        pygame.event.pump()
        getkey()
        if use_controller:
            throttleval = joysticks[2].get_axis(2)
            if joysticks[2].get_button(1) == 1:
                lowrange = True
            elif joysticks[2].get_button(2) == 1:
                lowrange = False
            if invertible:
                throttleval = int(63.5 - 63.5 * throttleval)
                if lowrange:
                    throttleval = int(throttleval/3)
                if reversed:
                    throttleval += 127
                else:
                    throttleval = 128 - throttleval
            else:
                throttleval = int(127.0 - 128.0 * throttleval)
                if lowrange:
                    throttleval = int(throttleval/3)
                if(throttleval <= 0):
                    throttleval = 1
        else:
            if invertible:
                throttleval = 127
            else:
                throttleval = 1
        if throttleval == 44:
            throttleval = throttleval + 1
        if firsttime:
            throttleval = 1
            if invertible:
                throttleval = 127
            firsttime = False
        leftspeed = int(leftspeed * 127 / 100.0 + 128)
        rightspeed = int(rightspeed * 127 / 100.0 + 128)
        print str(leftspeed) + " " + str(rightspeed) + " " + str(throttleval)
        if use_serial:
            s.send(bytearray([leftspeed, rightspeed, throttleval]))
            s.send(',')
        time.sleep(0.05)
