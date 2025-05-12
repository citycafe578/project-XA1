import pygame
from pygame.locals import *
import sys
import time

class Joysticker:
    def __init__(self):
        pygame.init()
        pygame.joystick.init()
        
        # if pygame.joystick.get_count() == 0:
        #     print("No joystick detected")
        #     quit()
    
        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()
        
    def read(self):    
        data0 = int(self.joystick.get_axis(0)*1000) + 1000
        data1 = 2000 - int(self.joystick.get_axis(1)*1000)-1000
        data2 = int(self.joystick.get_axis(2)*1000) + 1000
        data3 = 2000 - int(self.joystick.get_axis(3)*1000)-1000
        return data0, data1, data2, data3
    
    def run(self):
        #running = True
        #while running:
        for event in pygame.event.get():
            if event.type == QUIT:
                runnng = False
        data0, data1, data2, data3 = self.read()
        # print (data0, data1, data2, data3)
        pygame.time.Clock().tick(50)
        return [data0, data1, data2, data3]
        pygame.quit()
        
if __name__ == "__main__":
    joystick_reader = Joysticker()
    joystick_reader.run()
