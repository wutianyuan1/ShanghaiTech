import numpy as np
import matplotlib.pyplot as plt
from cv2 import cv2
import random

""" Helper function: read points from txt file """
"""======================================================"""  
def read_points(filename):
    pts = []
    with open(filename) as f:
        for line in f.read().split("\n"):
            s1, s2 = line.split()
            pts.append((int(float(s1)), int(float(s2))))
    return np.array(pts)


""" Helper function: Argmax """
""" That is to find the pixel with highest priority """
"""======================================================"""  
def argmax(d):
    val, res = 0, None
    for key in d:
        if d[key] >= val:
            val = d[key]; res = key
    return res


""" Create the mask by the points """
"""======================================================"""  
def create_mask(img, pts):
    maxc, maxr = max(pts[:, 0]), max(pts[:, 1])
    minc, minr = min(pts[:, 0]), min(pts[:, 1])
    mask = np.zeros((len(img), len(img[0])))
    for i in range(len(img)):
        for j in range(len(img[0])):
            if (not (j < maxc and j > minc)) or (not (i < maxr and i > minr)):
                continue
            jj, c = len(pts)-1, False
            for ii in range(len(pts)):
                if ( ((pts[ii, 1]>i) != (pts[jj, 1]>i)) and (j < (pts[jj, 0]-pts[ii, 0]) * (i-pts[ii, 1]) / (pts[jj, 1]-pts[ii, 1]) + pts[ii, 0])):
                    c = not c
                jj = ii
            if c:
                mask[i, j] = 255
    cv2.imwrite("mask_gray.png", mask)


""" Find the boundary  """
"""======================================================"""  
def find_boundary(img):
    img_ = img.copy()
    boundary = cv2.Laplacian(img_, cv2.CV_16SC3, ksize=3)
    boundary = cv2.convertScaleAbs(boundary)
    return cv2.threshold(boundary, 100, 255, cv2.THRESH_BINARY)[1]


""" Calculate the priority \Psi(P) """
"""======================================================"""  
def cal_priority(img, mask_img, boundary_img, show=False):
    boundary_pts = []
    boundary_img_ = boundary_img.copy()
    pos = np.where(boundary_img_ == 255)
    boundary_pts = [(pos[0][i], pos[1][i]) for i in range(len(pos[0]))]   
    half_ksize = 7
    priority = {}
    img_ = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    der_x = cv2.Sobel(img_, cv2.CV_16S, 1, 0, ksize=3).astype(np.float)
    der_y = cv2.Sobel(img_, cv2.CV_16S, 0, 1, ksize=3).astype(np.float)
    lap_x = cv2.Sobel(der_x, cv2.CV_16S, 1, 0, ksize=3).astype(np.float)
    lap_y = cv2.Sobel(der_y, cv2.CV_16S, 1, 0, ksize=3).astype(np.float)
    max_datapri = 0
    for pt in boundary_pts:
        square = mask_img[pt[0]-half_ksize : pt[0]+half_ksize+1, pt[1]-half_ksize : pt[1]+half_ksize+1].astype(np.float)
        known = 1 - (np.sum(square/255) / ((1 + 2*half_ksize)**2))
        dotprod = ((-der_y[pt])*lap_x[pt] + (der_x[pt])*lap_y[pt])
        if dotprod < 0:
            dotprod = ((der_y[pt])*lap_x[pt] + (-der_x[pt])*lap_y[pt])
        assert(dotprod >= 0)
        data_pri = np.sqrt(der_x[pt]**2 + der_y[pt]**2) * dotprod
        if data_pri > max_datapri:
            max_datapri = data_pri
        if show: priority[pt] = known*data_pri; continue;
        priority[pt] = known*np.log(data_pri)
    if show:
        for key in priority:
            boundary_img_[key] = priority[key]/max_datapri*1000
        plt.imshow(boundary_img_, cmap='gray')
        plt.show()
        exit()
    return priority


""" Calculate the similarity between 2 patches """
"""======================================================"""  
def similarity(img1, img2, mask_img, i, j, patch_size):
    if np.sum(mask_img[i:i+patch_size, j:j+patch_size]) != 0:
        return float('inf')
    return np.sum(np.abs(img1.astype(np.float) - img2.astype(np.float)))
    # return np.sum((img1.astype(np.float) - img2.astype(np.float))**2)


""" Update the image and mask """
""" We do the `inpainting' process in this step """
"""======================================================"""  
def update(img, mask_img, priority, patch_size=51, step=15, show=False, flag=-1):
    position = argmax(priority)
    upperleft = (position[0] - patch_size//2, position[1] - patch_size//2)
    square = img[upperleft[0]:upperleft[0]+patch_size, upperleft[1]:upperleft[1]+patch_size, :]
    minsim, minpos = float('inf'), None
    if flag != -1:
        if upperleft[0] + patch_size//2 < 330:
            start, end = 0, 200
        elif upperleft[0] + patch_size//2 > 330 and upperleft[0] + patch_size//2 < 380:
            start, end = 300, 400
        else:
            start, end = 500, len(img)
    else:
        start, end, step= 0, len(img), step*2
    for i in range(start, end, step//2):
        for j in range(0, len(img[0]), step//2):
            patch = img[i:i+patch_size, j:j+patch_size]
            if len(patch) != patch_size or len(patch[0]) != patch_size:
                continue
            sim = similarity(patch, square, mask_img, i, j, patch_size)
            if sim < minsim:
                minsim = sim
                minpos = (i, j)
    cover = img[minpos[0]:minpos[0]+patch_size, minpos[1]:minpos[1]+patch_size]
    if show:
        plt.imshow(cv2.cvtColor(cover, cv2.COLOR_BGR2RGB))
        plt.show()
        plt.imshow(cv2.cvtColor(img[upperleft[0]:upperleft[0]+patch_size, upperleft[1]:upperleft[1]+patch_size, :], \
                                cv2.COLOR_BGR2RGB))
        plt.show()
        tmp = np.zeros((patch_size, patch_size, 3), dtype=np.uint8)
        for i in range(patch_size):
            for j in range(patch_size):
                if (img[upperleft[0]+i, upperleft[1]+j] == [0, 0, 0]).all():
                    tmp[i, j] = cover[i, j]
                else:
                    tmp[i, j] = img[upperleft[0]+i, upperleft[1]+j]
        plt.imshow(cv2.cvtColor(tmp, cv2.COLOR_BGR2RGB))
        plt.show()
    img[upperleft[0]:upperleft[0]+patch_size, upperleft[1]:upperleft[1]+patch_size, :] = cover
    if show:
        plt.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
        plt.show()
        exit(0)
    mask_img[upperleft[0]:upperleft[0]+patch_size, upperleft[1]:upperleft[1]+patch_size] = 0


""" Inpainting the family image """
"""======================================================"""  
def family_inpainting():
    img = cv2.imread('./family.jpg')

    """======= Create the Mask ======="""
    """It will only be used in the first run, uncomment it to see the Mask"""
    # pts = read_points('./mask2.txt')
    # create_mask(img, pts)

    """======== Read the Mask ========"""
    mask_img = cv2.imread('./mask_gray2.png')
    mask_img = cv2.cvtColor(mask_img, cv2.COLOR_BGR2GRAY)
    img[np.where(mask_img != 0)] = [0, 0, 0]

    """========== Main Loop ==========="""
    while np.sum(mask_img) != 0:
        # print(np.sum(mask_img))
        boundary = find_boundary(mask_img)
        pri = cal_priority(img, mask_img, boundary)
        update(img, mask_img, pri, flag=1)
    plt.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
    plt.show()


""" Inpainting the grassland image """
"""======================================================"""  
def grassland_inpainting():
    img = cv2.imread('./grassland.jpg')

    """======= Create the Mask ======="""
    """It will only be used in the first run, uncomment it to see the Mask"""
    # pts = read_points('./mask.txt')
    # create_mask(img, pts)

    """======== Read the Mask ========"""
    mask_img = cv2.imread('./mask_gray1.png')
    mask_img = cv2.cvtColor(mask_img, cv2.COLOR_BGR2GRAY)
    img[np.where(mask_img != 0)] = [0, 0, 0]
    
    """========== Main Loop ==========="""
    while np.sum(mask_img) != 0:
        # print(np.sum(mask_img))
        boundary = find_boundary(mask_img)
        pri = cal_priority(img, mask_img, boundary)
        update(img, mask_img, pri)
    plt.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
    plt.show()


""" Main Entry """
"""======================================================"""  
if __name__ == '__main__':
    """Inpaint the grassland image"""
    # grassland_inpainting()
    """Inpaint the family image"""
    family_inpainting()
