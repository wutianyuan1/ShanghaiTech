from cv2 import cv2 
import matplotlib.pyplot as plt
import numpy as np
from collections import Counter
import heapq
from math import log, sqrt


""" Useful tables here """
"""======================================================"""  
zigzag_table = [[0,1,8,16,9,2,3,10], [17,24,32,25,18,11,4,5], [12,19,26,33,40,48,41,34], [27,20,13,6,7,14,21,28], 
                [35,42,49,56,57,50,43,36], [29,22,15,23,30,37,44,51], [58,59,52,45,38,31,39,46], [53,60,61,54,47,55,62,63]]

invzigzag_table = [[0,1,5,6,14,15,27,28], [2,4,7,13,16,26,29,42], [3,8,12,17,25,30,41,43], [9,11,18,24,31,40,44,53],
                [10,19,23,32,39,45,52,54], [20,22,33,38,46,51,55,60], [21,34,37,47,50,56,59,61], [35,36,48,49,57,58,62,63]]

y_table = [[16,11,10,16,24,40,51,61], [12,12,14,19,26,58,60,55], [14,13,16,24,40,57,69,56], [14,17,22,29,51,87,80,62],
            [18,22,37,56,68,109,103,77], [24,35,55,64,81,104,113,92], [49,64,78,87,103,121,120,101], [72,92,95,98,112,100,103,99]]

test_data = np.array([[134, 156, 160, 162, 163, 162, 149, 126], [134, 156, 160, 162, 163, 162, 149, 126], [134, 156, 160, 162, 163, 162, 149, 126], [134, 156, 160, 162, 163, 162, 149, 126],
            [134, 156, 160, 162, 163, 162, 149, 126], [134, 139, 144, 141, 135, 122, 104, 82], [125, 126, 121, 112, 95, 84, 71, 66], [114, 97, 87, 76, 74, 59, 63, 56]], dtype=np.float32)


""" Node of Huffman Tree """
"""======================================================"""  
class Node(object):
    def __init__(self, left, right):
        self.parent = None
        left.parent = right.parent = self
        self.left = left
        self.right = right
        self.weight = left.weight + right.weight

    def __lt__(self, other):
        return self.weight < other.weight


""" Leaf node of Huffman Tree """
"""======================================================"""  
class Leaf(Node):
    def __init__(self, symbol, weight):
        self.parent = None
        self.symbol = symbol
        self.weight = weight

    def code(self):
        code = ""; n = self
        while n.parent:
            codebit = "0" if n is n.parent.left else "1"
            code = codebit + code
            n = n.parent
        return code


""" Encoding `DATA` to truncated huffman code """
"""======================================================"""  
def truncated_huffman_encode(data, trunc_freq=0.0001):
    cnter = Counter(data)
    frequency = sorted(list(cnter.items()), key=lambda x: x[1])
    truncated_fq = 0; trunc = 0
    while frequency[trunc][1]/len(data) < 0.0001:
        truncated_fq += frequency[trunc][1]
        trunc += 1
    truncated = frequency[:trunc]
    other = frequency[trunc:]
    other.append((-1111, truncated_fq))
    leaves = [Leaf(*item) for item in other]
    heap = leaves[:]
    heapq.heapify(heap)
    while len(heap) >= 2:
        heapq.heappush(heap, Node(heapq.heappop(heap), heapq.heappop(heap))) 
    huffman_dict = {l.symbol: l.code() for l in leaves}
    trunc_id = huffman_dict[-1111]
    for i in range(len(truncated)):
        b = bin(i)[2:]
        code = trunc_id + '0'*(int(log(trunc))-len(b)) + b
        huffman_dict[truncated[i][0]] = code
    # Delete temp key used for truncate
    del huffman_dict[-1111]
    # The size of the compressed image
    ttl_len, l2 = 0, 0
    for key in huffman_dict:
        val = int(huffman_dict[key], 2)
        bits = log(val, 2)+1 if val != 0 else 1
        l2 += len(huffman_dict[key])*cnter[key]
        ttl_len += int(bits)*cnter[key]
    # The size of huffman dict
    return huffman_dict, ttl_len, l2


""" Read Y channel from the image corresponding to `FILENAME` """
"""======================================================"""  
def read_ychannel(filename):
    img = cv2.imread(filename)
    img_ycbcr = cv2.cvtColor(img, cv2.COLOR_BGR2YCrCb)
    (y, _, _) = cv2.split(img_ycbcr)
    return y.astype(np.float64)


""" Given an image, encode it to JPEG format """
"""======================================================"""  
def jpeg_encode(img):
    seq = ['']*(len(img)*len(img[0]))
    pos = 0
    for i in range(0, len(img), 8):
        for j in range(0, len(img[0]), 8):
            dct_res = cv2.dct(img[i:i+8, j:j+8])  
            dct_res = (dct_res/y_table).astype(np.int32)
            zigzag_res = np.zeros(64)
            for it in range(64):
                val = zigzag_table[it//8][it%8]
                zigzag_res[it] = dct_res[val//8, val%8]
            for k in range(64):              
                seq[pos+k] = zigzag_res[k]
            pos += 64
    huff_dict, l, l2 = truncated_huffman_encode(seq) 
    for i in range(len(seq)):
        seq[i] = huff_dict[seq[i]]
    return seq, huff_dict, l, l2


""" Given a sequence and huffman dict, decode the `JPEG` sequence """
"""======================================================"""  
def jpeg_decode(jpeg_seq, huff_dict):
    cnt = 0
    new_img = np.zeros((512,512))
    invdict = {}
    for key in huff_dict:
        invdict[huff_dict[key]] = key
    for i in range(0, 512, 8):
        for j in range(0, 512, 8):            
            subseq = jpeg_seq[cnt*64: cnt*64+64]
            for k in range(64):
                subseq[k] = invdict[subseq[k]]
            cnt += 1
            window = np.zeros((8, 8))
            for r in range(8):
                for c in range(8):
                    window[r, c] = subseq[invzigzag_table[r][c]]
            new_img[i:i+8, j:j+8] = cv2.idct(window*y_table)
    return new_img
    

""" Calculate 3 parameters """
"""======================================================"""  
def calc_param(length, ori, new):
    e_rms = sqrt(np.sum((new - ori)**2)/(512*512))
    snr = np.sum(ori**2) / np.sum((ori - new)**2)
    print("Root mean square error: %f" % e_rms)
    print("Mean-square signal-to-noise ratio: %f" % snr)
    print("Compressed ratio: {}".format((8*512*512)/(length)))


""" Main entry """
"""======================================================"""  
def main():
    ych = read_ychannel("./lena.tiff")
    plt.imshow(ych, cmap='gray')
    plt.title("Original Y channel")
    plt.show()
    seq, huff_dict, length, l2 = jpeg_encode(ych)
    new_img = jpeg_decode(seq, huff_dict)
    calc_param(l2, ych, new_img)
    plt.imshow(new_img, cmap='gray')
    plt.title("After Compression")
    plt.show()
    print("="*15 + "Huffman Code for given block" + "="*15)
    res, d, _, _ = jpeg_encode(test_data)
    print("Huffman encoding result:")
    print(res)
    print("Huffman code dictionary:")
    print(d)


if __name__ == "__main__":
    main()
