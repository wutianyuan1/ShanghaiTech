import math
from cv2 import cv2
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import Birch, KMeans
from skimage import io, color



""" Read input points """
"""======================================================"""  
def read_points(filename):
    pts = []
    with open(filename) as f:
        for line in f.read().split("\n"):
            s1, s2 = line.split(" ")
            pts.append((int(float(s2)), int(float(s1))))
    return pts
    

""" Show input points """
"""======================================================"""  
def show_points(img, pts):
    for pt in pts:
        cv2.rectangle(img, (pt[0]-3, pt[1]-3), (pt[0]+3, pt[1]+3), (255, 0, 0), 2)
    plt.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
    plt.show()


""" Region grow algorithm """
"""======================================================"""  
def region_grow(img, pts, threshold):
    h, w = len(img), len(img[0])

    def get_neighbor(x, y):
        res = [(x-1, y), (x+1, y), (x, y+1), (x, y-1)]
        return [item for item in res if (item[0] < h and item[1] < w and item[0] >= 0 and item[1] >= 0)]

    explored = np.ones((h, w))
    active = pts.copy()
    while len(active) != 0:
        point = active.pop(0)
        explored[point] = 0
        neighbors = get_neighbor(point[0], point[1])
        for n in neighbors:
            if explored[n] == 1 and (np.abs(img[n].astype(np.int) - img[point].astype(np.int)) < threshold).all():
                explored[n] = 0
                active.append(n)

    explored[200:, :] = cv2.erode(explored[200:, :], cv2.getStructuringElement(cv2.MORPH_ERODE, (4, 5)), iterations=2)
    explored[0:200, :] = cv2.erode(explored[0:200, :], cv2.getStructuringElement(cv2.MORPH_ERODE, (4, 4)), iterations=1)
    plt.imshow(explored, cmap='gray')
    plt.show()
    return cv2.add(img, np.zeros(np.shape(img), dtype=np.uint8), mask=explored.astype(np.uint8))


""" Definition of Cluster (Superpixel) """
"""======================================================"""  
class Cluster(object):
    cluster_index = 1
    def __init__(self, row, col, l=0, a=0, b=0):
        self.update(row, col, l, a, b)
        self.pixels = []
        self.no = self.cluster_index
        Cluster.cluster_index += 1

    def update(self, row, col, l, a, b):
        self.row, self.col, self.l, self.a, self.b = row, col, l, a, b


""" Definition of the superpixel class """
"""======================================================"""  
class SuperPixel(object):
    """ Check if a cluster is red """
    """======================================================"""  
    @staticmethod
    def is_red(cluster_rgb, row, col):
        return (cluster_rgb[0] > 60 and cluster_rgb[1] < 65 and cluster_rgb[2] < 70) or (cluster_rgb[0] > 190 and cluster_rgb[1] < 145)

    """ Check if a cluster is green """
    """======================================================"""  
    @staticmethod
    def is_green(cluster_rgb, row, col):
        if row < 310 and row > 260 and col < 300 and col > 210:
            return True
        if cluster_rgb[1] < 50 or cluster_rgb[2] > 150:
            return False
        return ((cluster_rgb[1] > cluster_rgb[0]) and (cluster_rgb[1] > cluster_rgb[2])) or \
        ((cluster_rgb[0] > 10 and cluster_rgb[0] < 175) and (cluster_rgb[1] > 65) and cluster_rgb[2] < 75 and abs(cluster_rgb[1] - cluster_rgb[0]) < 30)

    """ Check if a cluster is white """
    """======================================================"""  
    @staticmethod
    def is_white(cluster_rgb, row, col):
        if SuperPixel.is_green(cluster_rgb, row, col) or SuperPixel.is_red(cluster_rgb, row, col):
            return False
        return cluster_rgb[0] > 90 and cluster_rgb[1] > 90 and cluster_rgb[2] > 90 and abs(max(cluster_rgb) - min(cluster_rgb)) < 80

    """ Initialize method """
    """======================================================"""  
    def __init__(self, filename, K, M):        
        rgb = io.imread(filename)
        lab_arr = color.rgb2lab(rgb)
        # lab_arr = cv2.resize(lab_arr, (len(lab_arr[0])//2, len(lab_arr)//2) )
        self.data = lab_arr
        self.K = K
        self.M = M
        self.rows = self.data.shape[0]
        self.cols = self.data.shape[1]
        self.N = self.rows * self.cols
        self.S = int(np.sqrt(self.N / self.K))
        self.clusters = []
        self.label = {}
        self.dis = np.full((self.rows, self.cols), np.inf)

    """ Make a cluster """
    """======================================================"""  
    def make_cluster(self, row, col):
            row, col = int(row), int(col)
            return Cluster(row, col, self.data[row][col][0], self.data[row][col][1], self.data[row][col][2])

    """ Calculate the gradient """
    """======================================================"""  
    def pixel_grad(self, row, col):
        if col + 1 >= self.cols:
            col = self.cols - 2
        if row + 1 >= self.rows:
            row = self.rows - 2

        gradient = (self.data[row + 1][col][0] +self.data[row][col+1][0]-2*self.data[row][col][0])+ \
                   (self.data[row + 1][col][1] +self.data[row][col+1][1]-2*self.data[row][col][1]) + \
                   (self.data[row + 1][col][2] +self.data[row][col+1][2]-2*self.data[row][col][2])
        return gradient

    """ Move a cluster """
    """======================================================"""  
    def move(self):
        for cluster in self.clusters:
            cluster_gradient = self.pixel_grad(cluster.row, cluster.col)
            for dh in range(-1, 2):
                for dw in range(-1, 2):
                    _row = cluster.row + dh; _col = cluster.col + dw
                    new_gradient = self.pixel_grad(_row, _col)
                    if new_gradient < cluster_gradient:
                        cluster.update(_row, _col, self.data[_row][_col][0], self.data[_row][_col][1], self.data[_row][_col][2])
                        cluster_gradient = new_gradient

    """ Assign cluster centers to new positions """
    """======================================================"""  
    def cluster_assign(self):
        for cluster in self.clusters:
            for h in range(cluster.row - 2 * self.S, cluster.row + 2 * self.S):
                if h < 0 or h >= self.rows: continue
                for w in range(cluster.col - 2 * self.S, cluster.col + 2 * self.S):
                    if w < 0 or w >= self.cols: continue
                    L, A, B = self.data[h][w]
                    Dc = np.sqrt(math.pow(L - cluster.l, 2) + math.pow(A - cluster.a, 2) + math.pow(B - cluster.b, 2))
                    Ds = np.sqrt(math.pow(h - cluster.row, 2) + math.pow(w - cluster.col, 2))
                    D = np.sqrt(math.pow(Dc / self.M, 2) + math.pow(Ds / self.S, 2))
                    if D < self.dis[h][w]:
                        if (h, w) not in self.label:
                            self.label[(h, w)] = cluster
                            cluster.pixels.append((h, w))
                        else:
                            self.label[h, w].pixels.remove((h, w))
                            self.label[h, w] = cluster
                            cluster.pixels.append((h, w))
                        self.dis[h][w] = D

    """ Update a cluster """
    """======================================================"""  
    def update(self):
        for cluster in self.clusters:
            sum_h = sum_w = number = 0
            for p in cluster.pixels:
                sum_h += p[0]; sum_w += p[1]; number += 1
                _h =int( sum_h / number); _w =int( sum_w / number)
                cluster.update(_h, _w, self.data[_h][_w][0], self.data[_h][_w][1], self.data[_h][_w][2])

    """ Segmentation of pixels according to the results of superpixel """
    """======================================================"""  
    def color_segment(self, func):
        image_arr = np.copy(self.data)
        for cluster in self.clusters:            
            cluster_rgb = color.lab2rgb(np.array([[[cluster.l, cluster.a, cluster.b]]], dtype=np.float))
            cluster_rgb = cluster_rgb[0][0]*255
            if func(cluster_rgb, cluster.row, cluster.col):
                continue
            for p in cluster.pixels:
                image_arr[p[0], p[1]] = [0, 0, 0]     
        image_arr = color.lab2rgb(image_arr)
        plt.imshow(image_arr)
        plt.show()
                
    """ Main method of superpixel """
    """======================================================"""  
    def do_superpixel(self, func=is_red, loop=5):
        row = self.S / 2
        col = self.S / 2
        while row < self.rows:
            while col < self.cols:
                self.clusters.append(self.make_cluster(row, col))
                col+= self.S
            col = self.S / 2
            row += self.S
        self.move()
        for i in range(loop):
            print("SuperPixel Loop: {}/{}".format(i+1, loop))
            self.cluster_assign()
            self.update()
        self.color_segment(func)


""" The main entry """
"""======================================================"""  
if __name__ == '__main__':
    """Problem1 part1"""
    pts = read_points("./p1_bg.txt")
    img = cv2.imread("./pepper.png")
    res = region_grow(img, pts, 5)
    plt.imshow(cv2.cvtColor(res, cv2.COLOR_BGR2RGB))
    plt.show()
    """Problem1 part2"""
    p = SuperPixel("./p1_fg.jpg", 400, 40)
    p.do_superpixel(SuperPixel.is_red, 3)
    p = SuperPixel("./p1_fg.jpg", 400, 40)
    p.do_superpixel(SuperPixel.is_green, 3)
    p = SuperPixel("./p1_fg.jpg", 400, 40)
    p.do_superpixel(SuperPixel.is_white, 3)
        



