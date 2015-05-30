import pygame, serial
from pygame.locals import *

# Define colours
WHITE = (255, 255, 255)
GREEN = (0, 255, 0)
BLACK = (0, 0 , 0)

class Thp(object):

    def __init__(self):
        #self.arduino = serial.Serial('/dev/ttyUSB0', 9600)
        try: # Checks to see if the USB0 is avalible if not it uses USB1
            self.arduino = serial.Serial('/dev/ttyUSB0', 9600) # Arduino Serial port
        except:
            self.arduino = serial.Serial('/dev/ttyUSB1', 9600) # Arduino Serial port

    def taking_input(self):
        self.temp_c = self.arduino.read(6)
        if (self.temp_c.startswith("#1#") and self.temp_c.endswith(";")):
            print "temp_c - %s" % self.temp_c[3:-1]
            #break

    def heartbeat(self):
        #print("test")
        pygame.init()
        screen = pygame.display.set_mode((500, 250))
        pygame.display.set_caption('Temp C')
        background = pygame.Surface(screen.get_size())
        background = background.convert()
        background.fill((WHITE))
        
        font = pygame.font.Font(None, 60)
        text1 = font.render("#T_H_S", 1,(BLACK))
        titlepos = text1.get_rect()
        titlepos.centerx = background.get_rect().centerx
        #titlepos.centery = background.get_rect().centery
        
        reading = self.arduino.readline()
        font_heart = pygame.font.Font(None, 40)
        text_heart = font_heart.render("Heart rate - " , 1,(0, 250, 0))
        temppos_heart = text_heart.get_rect()
        temppos_heart.centerx = background.get_rect().centerx

        font_temp = pygame.font.Font(None, 40)
        text2 = font_temp.render("Body temp - %s" % self.temp_c[3:-1], 1,(0, 250, 0))
        temppos2 = text2.get_rect()
        temppos2.centerx = background.get_rect().centerx
        
        background.blit(text1, titlepos)
        background.blit(text2,(0,75))
        background.blit(text_heart,(0,125))
        screen.blit(background, (0, 0))
        pygame.display.flip()
        pygame.display.update()

    def run(self):
        while 1:
            self.taking_input()
            self.heartbeat()
            for event in pygame.event.get():
                if event.type == QUIT:
                    pygame.quit()
                    sys.exit()
        
if __name__ == '__main__':
    begin = Thp()
    begin.run()
