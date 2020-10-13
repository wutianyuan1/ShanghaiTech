import numpy as np
import matplotlib.pyplot as plt
from cv2 import cv2
from math import sqrt, log
from scipy.spatial.distance import squareform, pdist
from scipy import io
import random


""" Helper Functions """
"""======================================================"""
def scatter(img,x,y, idx):
    half_size = 3
    for i in range(x-half_size, x+half_size):
        for j in range(y-half_size, y+half_size):
            img[i, j] = c[idx]

def parse_lines(filename):
    f = open(filename)
    lines = f.read().split('\n')
    num_points = (len(lines)-4)//2
    f.close()
    dst_features = np.zeros((num_points, 2), dtype=np.int32)
    for i in range(num_points):
        sl = lines[i].split()
        dst_features[i][1], dst_features[i][0] = int(float(sl[0])), int(float(sl[1]))
    src_features = np.zeros((num_points, 2), dtype=np.int32)
    for i in range(num_points+4, len(lines)):
        sl = lines[i].split()
        idx = i-num_points-4
        src_features[idx][1], src_features[idx][0] = int(float(sl[0])), int(float(sl[1]))

    return dst_features, src_features


""" Show features (control points) """
"""======================================================"""
def show_features(srcft, dstft, srcimg, dstimg):
    for i in range(len(srcft)):
        scatter(srcimg, srcft[i][0], srcft[i][1], i)
    for i in range(len(dstft)):
        scatter(dstimg, dstft[i][0], dstft[i][1], i)
    fig = plt.figure()
    ax1 = fig.add_subplot(211)
    ax1.imshow(srcimg)
    ax2 = fig.add_subplot(212)
    ax2.imshow(dstimg)
    plt.show()


""" TPS Algorithm """
"""======================================================"""
def makeT(cp):
    K = cp.shape[0]
    T = np.zeros((K+3, K+3))
    for i in range(len(cp)):
        for j in range(len(cp)):
            if i == j:
                T[i, j] = 0
            else:
                dis = (cp[i][0] - cp[j][0])*(cp[i][0] - cp[j][0]) + (cp[i][1] - cp[j][1])*(cp[i][1] - cp[j][1])  
                T[i, j] = dis*log(dis)/2
    T[K:K+2, 0:K] = cp.T
    T[0:K, K:K+2] = cp
    T[K+2, 0:K] = np.ones(K)
    T[0:K, K+2] = np.ones(K).T
    return T


def tps_transform(gallery, probe):
    T = makeT(probe)
    tar_pt_xt = gallery[:, 0]
    tar_pt_yt = gallery[:, 1]
    xtAug = np.concatenate([tar_pt_xt, np.zeros(3)])
    ytAug = np.concatenate([tar_pt_yt, np.zeros(3)])
    cx = np.linalg.solve(T, xtAug)
    cy = np.linalg.solve(T, ytAug)
    return cx, cy


def do_TPS(src, dst, cx, cy, cps):
    K = len(cx)-3
    h, w = len(dst), len(dst[0]) 
    distances = np.ones((h,w), dtype=np.float64)
    for i in range(len(src)):
        for j in range(len(src[0])):
            sum_x, sum_y = 0, 0
            for k in range(K):
                dis = (i-cps[k][0])**2 + (j-cps[k][1])**2
                phi_ri = dis*log(dis)/2 if dis != 0 else 0
                sum_x += phi_ri*cx[k]
                sum_y += phi_ri*cy[k]
            sum_x = sum_x + cx[K]*i + cx[K+1]*j + cx[K+2]
            sum_y = sum_y + cy[K]*i + cy[K+1]*j + cy[K+2]
            if sum_x >= 0 and sum_y >= 0 and sum_x < h-1 and sum_y < w-1:
                xx, yy = int(sum_x), int(sum_y)
                for m in [xx, xx+1]:
                    for n in [yy, yy+1]:
                        r = (sum_x-m)**2 + (sum_y-n)**2
                        if distances[m, n] > r:
                            distances[m, n] = r
                            dst[m, n] = src[i, j]
        print(i)
    return dst


""" Linear transformation """
"""======================================================"""  
def linear_transform(image, pts1, pts2):
    grp_num = len(pts1)//3
    Ms = np.zeros((2, 3, grp_num))
    for i in range(0, grp_num*3, 3):
        M = cv2.getAffineTransform(pts1[i:i+3], pts2[i:i+3])
        Ms[:,:,i//3] = M
    M1 = np.zeros((2,3))
    for i in range(grp_num):
        M1 += Ms[:, :, i]
    M1 = M1/12.5
    warped = cv2.warpAffine(image, M1, (386, 503))
    return warped


""" Image registeration """
"""======================================================"""  
def get_registered_img(show=True):
    """If `show` is set to True, then show the image by plt"""
    srcimg = cv2.imread('./man.jpg')
    srcimg = cv2.cvtColor(srcimg, cv2.COLOR_BGR2RGB)
    dstimg = cv2.imread('./girl.jpeg')
    dstimg = cv2.cvtColor(dstimg, cv2.COLOR_BGR2RGB)
    dst, src = parse_lines('./points_TPS.txt')
    dst_lin, src_lin = parse_lines('./points_Linear.txt')
    """Show control points"""
    # show_features(src, dst, srcimg, dstimg)
    """Uncomment following lines to show the result of Linear Transformation"""
    """It has already been saved in `man_linear.jpg`"""
    # res = linear_transform(srcimg, np.array(src_lin, dtype=np.float32), np.array(dst_lin, dtype=np.float32))
    # plt.imshow(res)
    # plt.title("Man - after linear transform")
    # plt.show()
    # res = cv2.cvtColor(res, cv2.COLOR_RGB2BGR)
    # cv2.imwrite("man_linear.jpg", res)
    """TPS Transformation"""
    srcimg = cv2.imread('./man_linear.jpg')
    srcimg = cv2.cvtColor(srcimg, cv2.COLOR_BGR2RGB)
    cx, cy = tps_transform(dst, src)
    post = do_TPS(srcimg, dstimg, cx, cy, src)
    dstimg = cv2.imread('./girl.jpeg')
    dstimg = cv2.cvtColor(dstimg, cv2.COLOR_BGR2RGB)
    if show:
        post = post[0:384, 0:384, :]
        dstimg = dstimg[0:384, 0:384, :]
        post[:, 0:200, :] = dstimg[:, 0:200, :]
        plt.imshow(post)
        plt.show()
    post = cv2.cvtColor(post, cv2.COLOR_RGB2BGR)
    dstimg = cv2.cvtColor(dstimg, cv2.COLOR_RGB2BGR)
    return post[0:384, 0:384, :], dstimg[0:384, 0:384, :]


""" Image blending using Laplace pyramaid method """
"""======================================================""" 
def laplace_pyr(image, level=3):
    curr, res, gauss = image.copy(), [], []
    for _ in range(level+1):
        gauss.append(curr)
        curr = cv2.pyrDown(curr)
    for i in range(level):
        curr = gauss[i].astype(np.float32) - (cv2.pyrUp(gauss[i+1])).astype(np.float32)
        curr = np.clip(curr, 0, 255).astype(np.uint8)
        res.append(curr)
    res.append(gauss[-1])
    return res


""" Reconstruct image from Laplace pyramaid """
"""======================================================""" 
def reconstruct(lap_pyr):
    res = lap_pyr[0]
    for i in range(1, len(lap_pyr)):
        tmp = lap_pyr[i]
        for _ in range(i):
            tmp = cv2.pyrUp(tmp)
        res = tmp.astype(np.float32) + res.astype(np.float32)
        res = np.clip(res, 0, 255)
    return res.astype(np.uint8)


""" Image blending using Laplace pyramaid method """
"""======================================================""" 
def blending(option=1):
    """Option=1: girl at Left, boy at Right"""
    """Option=2: girl at Right, boy at Left"""
    man = cv2.imread("./TPS_res.jpg")
    girl = cv2.imread("./girl.jpeg")[0:384, 0:384, :]
    lpg = laplace_pyr(girl)
    lpb = laplace_pyr(man)
    lps = []
    for i in range(len(lpb)):
        tmp = np.zeros(lpb[i].shape)
        if option == 1: 
            tmp[:, 0:int(len(lpb[i])/1.8), :] = lpg[i][:, 0:int(len(lpb[i])/1.8), :]
            tmp[:, int(len(lpb[i])/1.8):, :] = lpb[i][:, int(len(lpb[i])/1.8):, :]
        else:
            tmp[:, 0:int(len(lpb[i])/2), :] = lpb[i][:, 0:int(len(lpb[i])/2), :]
            tmp[:, int(len(lpb[i])/2):, :] = lpg[i][:, int(len(lpb[i])/2):, :]
        lps.append(tmp)
    img = reconstruct(lps)
    cv2.imwrite("blending"+str(option)+".jpg", img)
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    plt.imshow(img)
    plt.show()


""" Main entry """
"""======================================================"""  
def main():
    """Uncomment following 2 lines to show the image registeration results"""
    """Now it's already saved in `TPS_res.jpg`"""
    # ps, _ = get_registered_img(False)
    # cv2.imwrite("TPS_res.jpg", ps)
    """Image blending using Laplace pyramaid method"""
    blending(2)


if __name__ == "__main__":
    main()