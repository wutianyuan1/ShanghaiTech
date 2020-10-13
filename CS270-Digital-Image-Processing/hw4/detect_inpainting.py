from cv2 import cv2
import numpy as np
import matplotlib.pyplot as plt
from itertools import chain

def phash(img):
    img = cv2.cvtColor(cv2.resize(img, (8, 8)), cv2.COLOR_BGR2GRAY)
    dct_res = cv2.dct(img.astype(np.float))
    seq = list(chain.from_iterable(dct_res))
    avg = float(sum(seq)) / len(seq)
    encoded_avg = "".join(['0' if i < avg else '1' for i in seq])
    return ''.join(['%x' % int(''.join(encoded_avg[x:x+4]), 2) for x in range(0, 8*8, 4)])

def detect(img):
    d = {}
    for i in range(0, len(img)-51, 2):
        for j in range(0, len(img[0])-51, 2):
            hash_ = phash(img[i:i+51, j:j+51])
            if hash_ not in d:
                d[hash_] = 1
            else:
                d[hash_] += 1
    return sorted(list(d.values()), reverse=True)

img = cv2.imread('./grassland.jpg')
img2 = cv2.imread('./inpainted.png')
print("Shape:", img.shape, img2.shape)
l1, l2 = detect(img), detect(img2)
print("Origin Image: {}".format(l2[0:10]))
print("Inpainted Image: {}".format(l1[0:10]))
