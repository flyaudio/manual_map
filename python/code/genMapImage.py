# -*- coding: utf-8 -*-

# base on environment "Forklift-calibration-tool"
import cv2
import numpy as np
import yaml
import math
from cameraClass import *
from enum import Enum

class color(Enum):
    unknown_color  = 205
    obstacle_color = 0    
    free_color     = 255    
    
def main():
    ymlFilePath = 'map.yml'
    warehouse = 'map_sf'
    line_width = 6 #pixel
    CalibrCam = FrontCamera(warehouse, ymlFilePath)
    scale = float(CalibrCam.cfg[warehouse]['scale'])
    map_size = map(eval,CalibrCam.cfg[warehouse]['map_size'].split(','))
    print('map_size = ', map_size)
    im_width = int(map_size[0] / scale)
    im_height = int(map_size[1] / scale)
    im = np.zeros((im_height, im_width), dtype=np.uint8) 
    im[:] = color.unknown_color.value
#    im = cv2.imread('map_sf_2.0cm_pixel.png');
     
    lines = CalibrCam.cfg[warehouse]['lines']
    print('lines = ', lines)
    for line in lines:
        l = line.split(',')
        x1,y1,x2,y2 = map(eval, l) #string list to int list
        cv2.line(im, (int(x1/scale),int(y1/scale)), (int(x2/scale),int(y2/scale)), (color.obstacle_color.value), line_width)
#        cv2.line(im, (int(x1/scale),int(y1/scale)), (int(x2/scale),int(y2/scale)), (255,0,0), line_width)
        
#    cv2.circle(im,(396/2,440/2),3,(255),-1)

#    result_img = cv2.resize(im,None,fx=0.5,fy=0.25,interpolation=cv2.INTER_CUBIC)#INTER_NEAREST, INTER_LINEAR, INTER_CUBIC
    result_img = cv2.resize(im,(1920,1080),interpolation=cv2.INTER_CUBIC)#INTER_NEAREST, INTER_LINEAR, INTER_CUBIC
    
# If you need to show an image that is bigger than the screen resolution, 
# you will need to call namedWindow("", WINDOW_NORMAL) before the imshow.
#    cv2.namedWindow("output", cv2.WINDOW_NORMAL) 
    cv2.imshow('output',result_img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    cv2.imwrite(warehouse + "_{}cm_pixel.pgm".format(scale), im)
#    cv2.imwrite(warehouse + "_{}cm_pixel.png".format(scale), im)

#    while(1):
#        cv2.imshow('result_img',result_img)
#        cv2.imshow('img',img)
#        if cv2.waitKey(1) & 0xFF == 27:
#            break
#    cv2.destroyAllWindows()


if __name__=='__main__':
    main()
