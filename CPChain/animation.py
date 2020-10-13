import pygame
from pygame.locals import *

class MySprite(pygame.sprite.Sprite):
    def __init__(self, target):
        pygame.sprite.Sprite.__init__(self)
        self.target_surface = target
        self.image = None
        self.master_image = None
        self.rect = None
        self.topleft = 0,0
        self.frame = 0
        self.old_frame = -1
        self.frame_width = 1
        self.frame_height = 1
        self.first_frame = 0
        self.last_frame = 0
        self.columns = 1
        self.last_time = 0

    def load(self, filename, width, height, columns):
        self.master_image = pygame.image.load(filename).convert_alpha()
        self.rect = self.master_image.get_rect()
        if (filename == "hu.png"):
            self.master_image = pygame.transform.scale(self.master_image,(2128, 1212))
            width *= 2
            height *= 2
            self.rect.topleft = (200,200)
        else:
            self.master_image = pygame.transform.scale(self.master_image,(width, height))
            self.rect.topleft = (500, 500)
        self.frame_width = width
        self.frame_height = height


        self.columns = columns
        rect = self.master_image.get_rect()
        self.last_frame = (rect.width // (width)) * (rect.height // (height)) - 1

    def update(self, current_time, rate=60):
        if current_time > self.last_time + rate:
            self.frame += 1
            if self.frame > self.last_frame:
                self.frame = self.first_frame
            self.last_time = current_time

        if self.frame != self.old_frame:
            frame_x = (self.frame % self.columns) * self.frame_width
            frame_y = (self.frame // self.columns) * self.frame_height
            rect = ( frame_x, frame_y, self.frame_width, self.frame_height )
            self.image = self.master_image.subsurface(rect)
            self.old_frame = self.frame
