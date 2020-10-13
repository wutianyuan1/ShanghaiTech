import data

'''
class Card(object):
    def __init__(self, text):
        #if (text not in txt2img):
            #raise KeyError
        self._text = text
        self._num = int(text[0])
        self._attr = text[1]

    def __str__(self):
        return str(self._text)

    def __repr__(self):
        return str(self._text)

    def __gt__(self, other):
        if  (self._attr > other._attr): return True
        elif(self._attr < other._attr): return False
        else: return self._num > other._num 
'''

card2num = {
    "m":0,"1m":0,"2m":1,"3m":2,"4m":3,"5m":4,"6m":5,"7m":6,"8m":7,"9m":8,
    "p":9,"1p":9,"2p":10,"3p":11,"4p":12,"5p":13,"6p":14,"7p":15,"8p":16,"9p":17,
    "s":18,"1s":18,"2s":19,"3s":20,"4s":21,"5s":22,"6s":23,"7s":24,"8s":25,"9s":26,
    "1z":27,"2z":28,"3z":29,"4z":30,"5z":31,"6z":32,"7z":33
}

class Analyser(object):
    def __init__(self, hand):
        self._hand = []
        for card in hand:
            self._hand.append(card._text)
        self.n = [0]*34
        for text in self._hand:
            num = card2num[text]
            self.n[num] += 1
        self._pos = [0]*14

    
    def cal_key(self):
        p = -1
        x = 0
        pos_p = 0
        b = False
        for i in range(3):
            for j in range(9):
                if (self.n[i * 9 + j] == 0):
                    if (b):
                        b = False
                        x |= 0x1 << p
                        p += 1
                else:
                    p += 1
                    b = True
                    self._pos[pos_p] = i * 9 + j
                    pos_p += 1
                    if (self.n[i * 9 + j] == 2):
                        x |= 0x3 << p
                        p += 2
                    elif (self.n[i * 9 + j] == 3):
                        x |= 0xF << p
                        p += 4
                    elif (self.n[i * 9 + j] == 4):
                        x |= 0x3F << p
                        p += 6
            if (b):
                b = False
                x |= 0x1 << p
                p = p + 1

        for i in range(27, 34):
            if (self.n[i] > 0):
                p = p + 1
                self._pos[pos_p] = i
                pos_p += 1
                if (self.n[i] == 2):
                    x |= 0x3 << p
                    p += 2
                elif (self.n[i] == 3):
                    x |= 0xF << p
                    p += 4
                elif (self.n[i] == 4):
                    x |= 0x3F << p
                    p += 6
                x |= 0x1 << p
                p = p + 1
        return x
    
    def get_res(self):
        try:
            ans = data.dic[self.cal_key()]
            return ans
        except KeyError:
            return -1
        
