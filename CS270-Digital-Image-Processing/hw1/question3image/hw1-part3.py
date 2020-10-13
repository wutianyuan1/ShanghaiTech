import numpy as np
import cv2
import matplotlib.pyplot as plt
import math

""" Read image and convert it to 8 color level """
"""======================================================"""  
def read_gray_img(filename):
    img = cv2.imread(filename)
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return (img/32).astype(np.uint8)



""" Get the co-occurance matrix """
"""======================================================"""  
def get_co_matrix(mat, _type):
    '''type=0: 0; type=1: 135, type=2: 90, type=3: 45'''
    ret = np.zeros((8,8), dtype=np.float64)
    if _type == 0:
        for i in range(len(mat)):
            for j in range(len(mat[0])-1):
                ret[mat[i,j], mat[i,j+1]] += 1
    elif _type == 1:
        for i in range(1, len(mat)):
            for j in range(len(mat[0])-1):
                ret[mat[i,j], mat[i-1,j+1]] += 1
    elif _type == 2:
        for i in range(1, len(mat)):
            for j in range(len(mat[0])):
                ret[mat[i,j], mat[i-1,j]] += 1
    elif _type == 3:
        for i in range(1, len(mat)):
            for j in range(1, len(mat[0])):
                ret[mat[i,j], mat[i-1,j-1]] += 1
    if np.sum(ret) == 0:
        return ret
    return ret/np.sum(ret)



""" Calculate contrast """
"""======================================================"""  
def contrast(img):
    contrasts = np.zeros((len(img), len(img[0]), 4), dtype=np.float32)
    square = np.zeros((8,8))
    for i in range(8):
        for j in range(8):
            square[i,j] = (i-j)*(i-j)
    for i in range(3, len(img)-3):
        for j in range(3, len(img[0])-3):
            window = img[i-3:i+4, j-3:j+4]
            for _type in range(4):
                comat = get_co_matrix(window, _type)
                contrasts[i, j, _type] = min(np.sum(square*comat), 1)
    return contrasts



""" Calculate correlation """
"""======================================================"""  
def correlation(img):
    corrs = np.zeros((len(img), len(img[0]), 4), dtype=np.uint8)
    ui_core, uj_core = np.zeros((8,8)), np.zeros((8,8))
    for i in range(8):
        ui_core[i, :] = i
        uj_core[:, i] = i
    for i in range(3, len(img)-3):
        for j in range(3, len(img[0])-3):
            window = img[i-3:i+4, j-3:j+4]
            for _type in range(4):
                comat = get_co_matrix(window, _type)
                ui = np.sum(ui_core*comat)
                uj = np.sum(uj_core*comat)
                tmpi = ui_core - ui
                tmpj = uj_core - uj
                sigmai = math.sqrt(np.sum(comat*tmpi*tmpi))
                sigmaj = math.sqrt(np.sum(comat*tmpj*tmpj))
                if sigmaj == 0 or sigmai == 0:
                    continue
                corrs[i, j , _type] = int(np.sum((comat*tmpi*tmpj)/sigmai/sigmaj)*255)
    return corrs



""" Calculate energy """
"""======================================================"""  
def energy(img):
    energys = np.zeros((len(img), len(img[0]), 4), dtype=np.float32)
    for i in range(3, len(img)-3):
        for j in range(3, len(img[0])-3):
            window = img[i-3:i+4, j-3:j+4]
            for _type in range(4):
                comat = get_co_matrix(window, _type)
                energys[i, j, _type] = np.sum(comat*comat)
    return energys



""" Calculate homogeneity """
"""======================================================"""  
def homogeneity(img):
    homos = np.zeros((len(img), len(img[0]), 4), dtype=np.float32)
    for i in range(3, len(img)-3):
        for j in range(3, len(img[0])-3):
            window = img[i-3:i+4, j-3:j+4]
            for _type in range(4):
                comat = get_co_matrix(window, _type)
                homo_pt = 0
                for ii in range(8):
                    for jj in range(8):
                        homo_pt += comat[ii,jj]/(1+abs(ii-jj))
                homos[i, j, _type] = min(homo_pt, 1)
    return homos



""" Show results """
"""======================================================"""  
def show(contrasts):
    avg = np.zeros((contrasts[:,:,0].shape))
    for i in range(4):
        avg += contrasts[:,:,i]
    avg /= 4
    plt.imshow(avg, cmap=plt.cm.gray)
    plt.show()
    fig = plt.figure()
    ax1 = fig.add_subplot(221)
    ax1.imshow(contrasts[:,:,0], cmap=plt.cm.gray)
    ax1 = fig.add_subplot(222)
    ax1.imshow(contrasts[:,:,3], cmap=plt.cm.gray)
    ax1 = fig.add_subplot(223)
    ax1.imshow(contrasts[:,:,2], cmap=plt.cm.gray)
    ax1 = fig.add_subplot(224)
    ax1.imshow(contrasts[:,:,1], cmap=plt.cm.gray)
    plt.show()



""" Main entry """
"""======================================================"""  
if __name__ == "__main__":
    img = read_gray_img('./Lenna.png')
    ret = correlation(img)
    show(ret)
    ret = contrast(img)
    show(ret)
    ret = energy(img)
    show(ret)
    ret = homogeneity(img)
    show(ret)


