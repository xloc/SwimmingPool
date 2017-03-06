'''
The code in the jupyter notebook
'''

__author__ = 'xloc'

import glob
import cv2
import random
import numpy as np
#from matplotlib import pyplot as plt  


def load_img(path, ratio=0.5, show=True):
    im = cv2.imread(path)

    h,w = im.shape[0:2]

    im = cv2.resize(im, dsize=(int(w*ratio), int(h*ratio)))

    im = cv2.cvtColor(im, cv2.COLOR_BGR2HSV)
   
    return im



def convertjpg(jpgfile):
    im = load_img(jpgfile, ratio=0.5, show=False)
    v = im[:,:,2]
    dx, dy = np.gradient(v)
    g = (dx**2 + dy**2)**0.5
    g = g/g.max()*255
    core = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5,5))
    g = cv2.erode(g, core)
    g = g.astype(dtype=np.uint8)
    g = cv2.equalizeHist(g)
    #core = cv2.getStructuringElement(cv2.MORPH_RECT, (2,2))
    #g = cv2.erode(g, core)
#    plt.imshow(g, cmap=plt.cm.gray)
#    row_g = g.shape[0]
#    g = g[row_g/2,:]
    i = str(random.uniform(10, 20)) 
    cv2.imwrite('E:/TDPS/Dataf/p/'+i+'.jpg', g)

for jpgfile in glob.glob("E:/TDPS/Dataf/*.jpg"):
    convertjpg(jpgfile)
    
    




