import numpy as np
from cv2 import cv2
import matplotlib.pyplot as plt
from collections import Counter

try:
    import maxflow
except ImportError:
    print ("*** Dear TA, if you cannot run my code, please install `pymaxflow` using pip first")
    print ("*** This is a module of finding the maximum flow in a graph")
    print ("*** I have looked through the source code, it's just a Ford-Fulkson algorithm written in C++")
    print ("*** As CS270 focus on image processing instead of implementation of classical algorithms, I think this module is free to use")
    exit(1)


""" Definition of graphcut """
"""======================================================"""  
class GraphCut:
    """ Initialize method """
    """======================================================""" 
    def __init__(self, img, fg, bg, sigma):
        self.image = img
        self.graph = np.zeros((len(self.image), len(self.image[0])))
        self.mask = None
        self.prior_bg = bg
        self.prior_fg = fg
        self.prob = 0
        self.sigma = sigma
        self.prior_maximum = 1e6
        self.nodes = []
        self.edges = []

    """ Convert (x,y) to index in graph """
    """======================================================""" 
    def xy2index(self, x, y):
        return y * self.image.shape[1] + x

    """ Convert index in graph to (x,y) """
    """======================================================""" 
    def index2xy(self, nodenum):
        return (nodenum//len(self.image[0]), nodenum%len(self.image[0]))

    """ Main entry of graphcut class """
    """======================================================""" 
    def do_graphcut(self, show, verbose=1):
        if verbose == 1:
            print("****** Graph Cut ******")
        self.add_Tlinks(verbose)
        self.add_Nlinks(verbose)
        return self.cut_graph(show, verbose)

    """ Adding T links """
    """======================================================""" 
    def add_Tlinks(self, verbose=1):
        if verbose == 1:
            print("*** Adding T links")
        self.graph = np.zeros((self.image.shape[0], self.image.shape[1]))
        self.graph.fill(0.5)
        self.hist_fg = Counter()
        self.hist_bg = Counter()      
        for (x, y) in self.prior_bg:
            self.graph[y-1, x-1] = 0
            p = self.image[y-1, x-1]
            self.hist_bg[int(p[0] * 0.114 + p[1] * 0.587 + p[2] * 0.299)] += self.prob
        for (x, y) in self.prior_fg:
            self.graph[y-1, x-1] = 1
            p = self.image[y-1, x-1]
            self.hist_fg[int(p[0] * 0.114 + p[1] * 0.587 + p[2] * 0.299)] += self.prob
        for key in range(256):
            self.hist_fg[key] /= len(self.prior_fg)
        for key in range(256):
            self.hist_bg[key] /= len(self.prior_bg)

    """ Adding N links """
    """======================================================""" 
    def add_Nlinks(self, verbose):
        if verbose == 1:
            print ("*** Adding N links")
        self.nodes = []
        self.edges = []
        for (y, x), value in np.ndenumerate(self.graph):
            if value == 0:
                self.nodes.append((self.xy2index(x, y), self.prior_maximum, 0))
            elif value == 1:
                self.nodes.append((self.xy2index(x, y), 0, self.prior_maximum))
            else:
                p = self.image[y, x]
                gray = int(p[0] * 0.114 + p[1] * 0.587 + p[2] * 0.299)
                self.nodes.append((self.xy2index(x, y), self.hist_bg[gray], self.hist_fg[gray]))

        for (y, x), value in np.ndenumerate(self.graph):
            if x == 0 or y ==0 or y == self.graph.shape[0] - 1 or x == self.graph.shape[1] - 1:
                continue
            curr_idx = self.xy2index(x, y)
            next_idx = self.xy2index(x+1, y)
            weight = np.exp(-np.sum((self.image[y, x] - self.image[y, x+1].astype(np.float64))**2)/(2*self.sigma*self.sigma) )
            self.edges.append((curr_idx, next_idx, weight))
            next_idx = self.xy2index(x, y+1)
            weight = np.exp(-np.sum((self.image[y, x] - self.image[y+1, x].astype(np.float64))**2)/(2*self.sigma*self.sigma) )
            self.edges.append((curr_idx, next_idx, weight))

    """ Find the min-cut """
    """======================================================""" 
    def cut_graph(self, show, verbose):
        if verbose == 1:
            print ("*** Finding maxflow")
        self.mask = np.zeros_like(self.image, dtype=bool)
        g = maxflow.Graph[float](len(self.nodes), len(self.edges))
        nodelist = g.add_nodes(len(self.nodes))
        for node in self.nodes:
            g.add_tedge(nodelist[node[0]], node[1], node[2])
        for edge in self.edges:
            g.add_edge(edge[0], edge[1], edge[2], edge[2])
        g.maxflow()
        for index in range(len(self.nodes)):
            if g.get_segment(index) == 1:
                x, y = self.index2xy(index)
                self.mask[x, y] = (True, True, True)
        res = np.zeros_like(self.image)
        np.copyto(res, self.image, where=self.mask)
        if show:
            plt.imshow(self.mask.astype(np.float))
            plt.show()
            plt.imshow(cv2.cvtColor(res, cv2.COLOR_BGR2RGB))
            plt.show()
        return self.mask


""" Read the input points """
"""======================================================""" 
def read_points(filename):
    pts = []
    with open(filename) as f:
        for line in f.read().split("\n"):
            s1, s2 = line.split(" ")
            pts.append((int(float(s1)), int(float(s2))))
    return pts

""" Do the multi-class graphcut """
"""======================================================""" 
def multi_class_graphcut(img, labels, show_points=False):
    if show_points:
        cp_img = img.copy()
        cp_img = cv2.cvtColor(cp_img, cv2.COLOR_BGR2RGB)
        color_1 = [[255,0,0], [0,255,0], [0,0,255], [255,255,0]]
        for i, label in enumerate(labels):
            for (x, y) in label:
                cv2.circle(cp_img, (x, y), 2, color_1[i], -1)
        plt.imshow(cp_img)
        plt.title("Points selected in different labels")
        plt.show()
    
    res = np.zeros_like(img)
    R = np.zeros_like(img)
    G = np.zeros_like(img)
    B = np.zeros_like(img)
    Y = np.zeros_like(img)
    R[:,:,0].fill(255)
    G[:,:,1].fill(255)
    B[:,:,2].fill(255)
    Y[:,:,0].fill(255)
    Y[:,:,1].fill(255)
    colors = [R, G, B, Y]
    for i in range(len(labels)):
        fg = labels[i]
        bg = []
        for j in range(len(labels)):
            if j != i:
                bg += labels[j]
        gc = GraphCut(img, fg, bg, 2.5)
        tmp = gc.do_graphcut(False, 0)
        np.copyto(res, colors[i], where=tmp)
    plt.imshow(res)
    plt.show()
    overlapping = cv2.addWeighted(cv2.cvtColor(img, cv2.COLOR_BGR2RGB), 0.6, res, 0.4, 0)
    plt.imshow(overlapping)
    plt.show()

""" Do graphcut """
"""======================================================""" 
def singel_graphcut(img, fg, bg):
    gc = GraphCut(img, fg, bg, 2.5)
    gc.do_graphcut(True)

""" Main entry """
"""======================================================""" 
def main():
    bg  = read_points('./bg.txt')
    fg  = read_points('./fg.txt')
    f1  = read_points('./fg21.txt')
    f2  = read_points('./fg22.txt')
    f3  = read_points('./fg23.txt')
    img = cv2.imread("./Pyy.jpg")
    """Sub-problem 1"""
    singel_graphcut(img, fg, bg)
    """Sub-problem 2"""
    multi_class_graphcut(img, [f1, f2, f3, bg])


if __name__ == '__main__':
    main()