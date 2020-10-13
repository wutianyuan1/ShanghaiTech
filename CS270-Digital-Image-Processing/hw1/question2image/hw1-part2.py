import numpy as np
import matplotlib.pyplot as plt
from cv2 import cv2
from math import sqrt, log
from scipy.spatial.distance import squareform, pdist

scalar = 5



""" Helper Functions """
"""======================================================"""
def scatter(img,x,y):
    half_size = 3
    for i in range(x-half_size, x+half_size):
        for j in range(y-half_size, y+half_size):
            img[i, j] = [255,0,0]

def parse_lines(filename):
    f = open(filename)
    lines = f.read().split('\n')
    num_points = (len(lines)-4)//2
    f.close()
    dst_features = np.zeros((num_points, 2), dtype=np.int32)
    for i in range(num_points):
        sl = lines[i].split()
        dst_features[i][1], dst_features[i][0] = int(1000/scalar*float(sl[0])), int(1000/scalar*float(sl[1]))
    src_features = np.zeros((num_points, 2), dtype=np.int32)
    for i in range(num_points+4, len(lines)):
        sl = lines[i].split()
        idx = i-num_points-4
        src_features[idx][1], src_features[idx][0] = int(1000/scalar*float(sl[0])), int(1000/scalar*float(sl[1]))
    return dst_features, src_features



""" Show features (control points) """
"""======================================================"""

def show_features(srcft, dstft, srcimg, dstimg):
    for i in range(len(srcft)):
        scatter(srcimg, srcft[i][0], srcft[i][1])
    for i in range(len(dst)):
        scatter(dstimg, dstft[i][0], dstft[i][1])
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
                # dst[int(sum_x):int(sum_x+2), int(sum_y):int(sum_y+2)] = src[i, j]
                xx, yy = int(sum_x), int(sum_y)
                for m in [xx, xx+1]:
                    for n in [yy, yy+1]:
                        r = (sum_x-m)**2 + (sum_y-n)**2
                        if distances[m, n] > r:
                            distances[m, n] = r
                            dst[m, n] = src[i, j]
        print(i)
    return dst



""" Main entry """
"""======================================================"""  

if __name__ == "__main__":
    srcimg = cv2.imread('./1.jpg')
    srcimg = cv2.cvtColor(srcimg, cv2.COLOR_BGR2RGB)
    srcimg = cv2.resize(srcimg, (int(4000/scalar), int(2250/scalar)))
    dstimg = cv2.imread('./2.jpg')
    dstimg = cv2.cvtColor(dstimg, cv2.COLOR_BGR2RGB)
    dstimg = cv2.resize(dstimg, (int(4000/scalar), int(2250/scalar)))
    dst, src = parse_lines('./part2points.txt')

    """Uncomment following line to show control points"""
    # show_features(src, dst, srcimg, dstimg)

    cx, cy = tps_transform(dst, src)
    post = do_TPS(srcimg, dstimg, cx, cy, src)

    plt.imshow(post)
    plt.show()

