import scipy.io as scio
import numpy as np
import matplotlib.pyplot as plt
from cv2 import cv2


""" Helper Functions - Read, Write and Show images"""
"""======================================================"""
def show_gray_img(img, title, savename=None):
    plt.imshow(img, cmap='gray')
    plt.title(title)
    if savename != None:
        plt.savefig(savename)
    plt.show()


def readmat():
    wuyuan = scio.loadmat("Wuyuan.mat")['Wuyuan'].astype(np.float32)
    logo   = scio.loadmat("LOGO_CS270.mat")['LOGO_CS270'].astype(np.uint8)
    return wuyuan, logo


""" Add the LOGO by using DCT """
"""======================================================"""
def add_logo(img, logo, alpha=2.0):
    dct_res = cv2.dct(img)
    logo = logo.astype(np.float64)/255
    d = {}
    for i in range(len(dct_res)):
        for j in range(len(dct_res[0])):
            d[dct_res[i,j]] = (i,j)
    cis = sorted(d.items(), key=lambda x: abs(x[0]), reverse=True)
    cnt = 0
    for i in range(len(logo)):
        for j in range(len(logo[0])):
            cnt += 1
            pos = cis[cnt][1]
            dct_res[pos] = dct_res[pos]*(1 + alpha*logo[i, j])
    ret = cv2.idct(dct_res)
    return ret/np.max(np.abs(ret))


""" Extract the LOGO from the watermarked image """
"""======================================================"""
def extract_logo(new_img, ori_img, shape, alpha=2.0):
    logo = np.zeros(shape)
    dct_res = cv2.dct(ori_img)
    dct_res2 = cv2.dct(new_img)
    d = {}
    for i in range(len(dct_res)):
        for j in range(len(dct_res[0])):
            d[dct_res[i,j]] = (i,j)
    cis = sorted(d.items(), key=lambda x: abs(x[0]), reverse=True)
    cnt = 0
    for i in range(len(logo)):
        for j in range(len(logo[0])):
            cnt += 1
            pos = cis[cnt][1]
            logo[i, j] = (dct_res2[pos]/dct_res[pos]-1)/alpha
    return logo


""" Add random Gaussian Noise """
"""======================================================"""
def gauss_noise(img, mean, var):
    noise = np.random.normal(mean, np.sqrt(var), img.shape)
    return img + noise


""" Main entry """
"""======================================================"""
def main():
    """Read original image"""
    wuyuan, logo = readmat()
    show_gray_img(wuyuan, "Origin image - Wuyuan.mat")

    """Add the logo"""
    res = add_logo(wuyuan, logo)
    show_gray_img(res, "After add LOGO", "Shangrao.png")

    """Extract the logo"""
    logo_ex = extract_logo(res, wuyuan, logo.shape)
    show_gray_img(logo_ex, "Extracted LOGO", "extracted_logo.png")

    """Add Gaussian noise"""
    wuyuan_noise = gauss_noise(res, 0, 0.001)
    show_gray_img(wuyuan_noise, "After add Noise", "SRnoise.png")

    """Extract logo from dirty image"""
    logo_ex2 = extract_logo(wuyuan_noise, wuyuan, logo.shape)
    show_gray_img(logo_ex2, "Extracted LOGO from Dirty Image", "extracted_noise_logo.png")


if __name__ == "__main__":
    main()   
      
