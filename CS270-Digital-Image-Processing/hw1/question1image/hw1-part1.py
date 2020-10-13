import cv2
import numpy as np 
import matplotlib.pyplot as plt 
import math
from collections import Counter


""" Helper Functions """
"""======================================================"""

def float2uint8(img):
    origin = cv2.split(img)
    ret = [np.array(origin[0], dtype=np.uint8), np.array(origin[1], dtype=np.uint8), np.array(origin[2], dtype=np.uint8)]
    for ch in range(3):
        for i in range(len(ret[0])):
            for j in range(len(ret[0][0])):
                ret[ch][i][j] = max(0, min(origin[ch][i][j]*255, 255))
    return cv2.merge(ret)

def img_padding(img, size):
    res = np.zeros((len(img)+size*2, len(img[0])+size*2, 3), dtype=np.uint8)
    res[size:size+len(img), size:size+len(img[0])] = img
    for i in range(size):
        for j in range(len(img[0])):
            res[i, j] = res[size+i ,j]
    for i in range(len(img)+size, len(img)+size*2):
        for j in range(len(img[0])):
            res[i, j] = res[2*(size+len(img))-i ,j]
    for i in range(size, size+len(img)):
        for j in range(0, size):
            res[i, j] = res[i, 2*size-j]
        for j in range(size+len(img[0]), 2*size+len(img[0])):
            res[i, j] = res[i, 2*(size+len(img[0]))-j]
    return res



""" Histogram equalization Algorithm """
"""======================================================"""

def single_channel_HE(channel):
    hist = Counter()
    for line in channel:
        hist += Counter(line)
    Tr = np.zeros((256))
    curr = 0
    for i in range(256):
        curr += hist[i]
        Tr[i] = int(curr*255/channel.size)
    for i in range(len(channel)):
        for j in range(len(channel[i])):
            channel[i, j] = Tr[channel[i, j]]
    return channel

def HE(img):
    splited = cv2.split(img)
    for i in range(len(splited)):
        channel = splited[i]
        single_channel_HE(channel)
    return cv2.merge(splited)



""" CLHE & CLAHE Algorithm """
"""======================================================"""

def single_channel_CLHE(channel, threshold=0.02):
    hist = Counter()
    for line in channel:
        hist += Counter(line)
    Tr = np.zeros((256))
    add = sum([hist[key] for key in hist if hist[key]/channel.size > threshold])
    cnt = sum([1 for key in hist if hist[key]/channel.size > threshold])
    if cnt != 0:
        add = int(add/cnt)
    for key in hist:
        if hist[key]/256 < threshold:
            hist[key] += add
    curr = 0
    for i in range(256):
        curr += hist[i]
        Tr[i] = int(curr*255/channel.size)
    for i in range(len(channel)):
        for j in range(len(channel[i])):
            channel[i, j] = min(max(Tr[channel[i, j]], 0), 255)
    return channel

def CLHE(img, threshold):
    splited = cv2.split(img)
    for i in range(len(splited)):
        channel = splited[i]
        single_channel_CLHE(channel, threshold)
    return cv2.merge(splited)

def CLAHE(img, window_size=33, threshold=0.02):
    """`img` is cv2 image, (B,G,R) encoded"""
    half_size = window_size//2
    splited = cv2.split(img_padding(img, half_size))
    ttl_res = []
    step = window_size//10
    coef = (window_size/step)**2
    for ch in range(3):
        channel = splited[ch].astype(np.float32)
        foo = np.zeros(channel.shape, dtype=np.float32)
        for i in range(0, len(channel)-window_size, step):
            for j in range(0, len(channel[0])-window_size, step):
                window = channel[i:i+window_size, j:j+window_size].copy()
                res = single_channel_CLHE(window.astype(np.uint8), threshold)
                foo[i:i+window_size, j:j+window_size] += (res/coef)
            print(i)
        foo = cv2.min(cv2.max(foo, 0), 255)
        ttl_res.append(foo.astype(np.uint8)[half_size:len(img)+half_size, half_size:len(img[0])+half_size])
    return cv2.merge(ttl_res)



""" Dark Channel Prior Algorithm """
"""======================================================"""

def min_value_filter(image, ksize=3):
    img = image.copy()
    rows, cols = img.shape
    padding = int((ksize-1)/2)
    new_img = cv2.copyMakeBorder(img, padding, padding, padding, padding, cv2.BORDER_CONSTANT, value=255)
    for i in range(rows):
        for j in range(cols):
            roi_img = new_img[i:i+ksize, j:j+ksize].copy()
            min_val,_,_,_ = cv2.minMaxLoc(roi_img) 
            img[i, j] = min_val
    return img

def get_A(img, percent = 0.001):
    mean_perpix = np.mean(img, axis = 2).reshape(-1)
    mean_topper = mean_perpix[:int(img.shape[0] * img.shape[1] * percent)]
    return np.mean(mean_topper)
    
def get_dark_channel(img, size=15):
    (b, g, r) = cv2.split(img)
    min_img = cv2.min(r, cv2.min(g, b))
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (size, size))
    return cv2.erode(min_img,kernel)

def dark_channel_prior(img, min_filter_size=15, w=0.95, t0=0.1):
    A = get_A(img)
    tx = 1-w*get_dark_channel(img/A, min_filter_size)
    tx = (cv2.max(cv2.min(tx*255, 255), 0)).astype(np.uint8)
    tx = cv2.bilateralFilter(tx,25, 255, 12.5)
    tx  = tx.astype(np.float64)/255
    for i in range(len(tx)):
        for j in range(len(tx[0])):
            if i < len(tx)//4:
                tx[i,j] = max(tx[i,j], 0.2-0.1*i/(len(tx)//4))
            else:
                tx[i,j] = max(tx[i,j], t0)
    res = np.zeros((img.shape[0], img.shape[1], 3), dtype=np.uint8)
    for ch in range(3):
        res[:,:,ch] = cv2.max(0, cv2.min((img[:,:,ch]-A)/tx + A, 255))
    return res



""" Gamma Transformation """
"""======================================================"""

def gamma_trans(img, gamma, eps, shift=0):
    gamma_table = [min(max(np.power((x/255.0+eps), gamma)*255.0 - shift, 0),255) for x in range(256)]
    gamma_table = np.round(np.array(gamma_table)).astype(np.uint8)
    return cv2.LUT(img, gamma_table)



""" Inverse Contrast Stretch Transformation """
"""======================================================"""

def inv_contrast_stretch(img, gamma):
    b, g, r = cv2.split(img)
    bm, gm, rm, _ = cv2.mean(img)
    table_b = [min(255, 80+bm/(np.power(1-(x/255.0), gamma))) for x in range(256)]
    table_b = np.round(np.array(table_b)).astype(np.uint8)
    table_g = [min(255, 80+gm/(np.power(1-(x/255.0), gamma))) for x in range(256)]
    table_g = np.round(np.array(table_g)).astype(np.uint8)
    table_r = [min(255, 60+rm/(np.power(1-(x/255.0), gamma))) for x in range(256)]
    table_r = np.round(np.array(table_r)).astype(np.uint8)
    return cv2.merge((cv2.LUT(b, table_b), cv2.LUT(g, table_g), cv2.LUT(r, table_r)))



""" Main Entry """
"""======================================================"""

def main():
    img = cv2.imread('./Ex_ColorEnhance.png')
    tam = cv2.imread('./Tam_clear.jpg')


    """Uncomment the following lines to get different results produced by different algorithms"""
    '''=============Color Enhance============='''
    '''Histogram equalization'''
    res = HE(img)
    '''CLAHE'''
    # res = CLAHE(img, 33, 0.015)
    '''CLHE'''
    # res = CLHE(img, 0.013)
    '''Dark channel prior (with bilateral filter) '''
    # res = dark_channel_prior(img)
    '''CLAHE + Gamma transformation'''
    # res = CLAHE(gamma_trans(img, 4.0))

    '''=================Add Fog=================='''
    # res = gamma_trans(tam, 0.2, 0, 0)

    # res = inv_contrast_stretch(tam, 0.4)

    # res = gamma_trans(tam, 0.2, 0, 60)
    
    plt.imshow(cv2.cvtColor(res, cv2.COLOR_BGR2RGB))
    plt.show()

if __name__ == "__main__":
    main()

