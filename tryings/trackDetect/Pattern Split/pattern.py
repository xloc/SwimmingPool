'''
The code in the jupyter notebook
'''

__author__ = 'xloc'

import glob
import cv2
#import random
import numpy as np
  
global image_array
global label_array
image_array = np.zeros((1, 30600))
label_array = np.zeros((1, 4), 'float')
k = np.zeros((4, 4), 'float')
for i in range(4):
    k[i, i] = 1


def load_img(path, ratio=0.25, show=True):
    im = cv2.imread(path)

    h,w = im.shape[0:2]

    im = cv2.resize(im, dsize=(int(w*ratio), int(h*ratio)))

    im = cv2.cvtColor(im, cv2.COLOR_BGR2HSV)
   
    return im



def convertjpg(jpgfile):
    global image_array
    global label_array
    
    im = load_img(jpgfile, ratio=0.25, show=False)
    v = im[:,:,2]
    dx, dy = np.gradient(v)
    g = (dx**2 + dy**2)**0.5
    g = g/g.max()*255
    core = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5,5))
    g = cv2.erode(g, core)
    g = g.astype(dtype=np.uint8)
    g = cv2.equalizeHist(g)
    core = cv2.getStructuringElement(cv2.MORPH_RECT, (2,2))
    g = cv2.erode(g, core)
    g = g[150:320,:]
    
    
    #存储路径
    temp_array = g.reshape(1, 30600).astype(np.float32)
    image_array = np.vstack((image_array, temp_array))
    
    
    
    
#原图路径
for jpgfile in glob.glob("E:/TDPS/Dataf/*.jpg"):
    convertjpg(jpgfile)



for i in range(36):
    print (" %d"%(i))
    m = int(raw_input())
    temp_lable = k[m]
    label_array = np.vstack((label_array, temp_lable))
    
    
    
    
train = image_array[1:, :]   
train_labels = label_array[1:, :]
#包含测试数据的文件（需要重命名，可以自己加路径）
np.savez('test08.npz', train=train, train_labels=train_labels)
print train.shape
    




