import sys
import pygame
import hashlib
from pygame.locals import *
import cal_win
import animation
import random
# import deploy


CARD_WIDTH  = 40
CARD_HEIGHT = 60
HAND_TOP    = 720
HAND_LEFT   = 240
HAND_RIGHT  = 900

STATUS_DEFAULT      = 0
STATUS_PLAYINTURN   = 1
STATUS_PLAYEXTURN   = 2
STATUS_DECISIONCHI  = 3
STATUS_CHOICECHI2   = 4
STATUS_CHOICECHI3   = 5
STATUS_DECISIONPENG = 6
STATUS_DECISIONGANG = 7
STATUS_END          = 96
    
class CHIError(Exception):
    pass
class PENGError(Exception):
    pass
class GANGError(Exception):
    pass
class HUError(Exception):
    pass
class EmptyError(Exception):
    pass

txt2img = {"1s":'1s.gif',"2s":'2s.gif',"3s":'3s.gif',"4s":'4s.gif',"5s":'5s.gif',"6s":'6s.gif',"7s":'7s.gif',"8s":'8s.gif',"9s":'9s.gif',
           "1m":'1m.gif',"2m":'2m.gif',"3m":'3m.gif',"4m":'4m.gif',"5m":'5m.gif',"6m":'6m.gif',"7m":'7m.gif',"8m":'8m.gif',"9m":'9m.gif',
           "1p":'1p.gif',"2p":'2p.gif',"3p":'3p.gif',"4p":'4p.gif',"5p":'5p.gif',"6p":'6p.gif',"7p":'7p.gif',"8p":'8p.gif',"9p":'9p.gif',
           "1z":'1z.gif',"2z":'2z.gif',"3z":'3z.gif',"4z":'4z.gif',"5z":'5z.gif',"6z":'6z.gif',"7z":'7z.gif'}



class Allcards(object):
    def __init__(self):
        self._content = 4* ["1s","2s","3s","4s","5s","6s","7s","8s","9s",
                            "1m","2m","3m","4m","5m","6m","7m","8m","9m",
                            "1p","2p","3p","4p","5p","6p","7p","8p","9p",
                            "1z","2z","3z","4z","5z","6z","7z"]   
        self._order = None
        self._md5   = None

    def shuffle(self):
        random.shuffle(self._content)
        self._order = "".join(item for item in self._content)
        return self._order
    
    def encode(self):
        code = hashlib.md5()
        code.update(self._order.encode("utf-8"))
        self._md5 = code.hexdigest()
        return self._md5

class Card(object):
    def __init__(self, text):
        #if (text not in txt2img):
            #raise KeyError
        self._text = text
        self._num = int(text[0])
        self._attr = text[1]
        self._img = pygame.image.load(self._text + ".gif").convert()
        self._img = pygame.transform.scale(self._img, (CARD_WIDTH, CARD_HEIGHT))

    def __str__(self):
        return str(self._text)

    def __repr__(self):
        return str(self._text)

    def __gt__(self, other):
        if  (self._attr > other._attr): return True
        elif(self._attr < other._attr): return False
        else: return self._num > other._num 
    
class Player(object):
    def __init__(self, namae, path, addr, pwd):
        self._hand = []
        self._name = namae
        self._score = None #????????????????????????????
        self._nummingcards = 0
        self._ming = []
        self._gang = []
        self.path  = ""
        self.addr  = ""
        self.pwd   = ""

    def getcard(self, crd):
        self._hand.append(crd)

    def playcard(self, cd):
        try:
            self._hand.remove(cd)
        except IndexError:
            pass
        self._hand.sort()

    def mingcard(self, cards):
        (c1, c2, c3) = cards
        self._ming += (sorted([c1, c2, c3]))
        self._nummingcards += 3

class Game(object):

    def __init__(self, p1, p2, p3, p4):
        pygame.init()
        self._mountain = Allcards()
        self._mountain.shuffle()
        self._mountain.encode()
        self._currnum = 0
        self._currplayer = 0
        self._players = [p1, p2, p3, p4]
        self._screen = pygame.display.set_mode([1000, 800], 0, 32)
        self._history = [[], [], [], []]
        self._background = pygame.image.load('back.png')
        self._background = pygame.transform.scale(self._background, (1000, 800))
        self._status = STATUS_DEFAULT
        #self._handpos = []
        self.display()
        self._clock = pygame.time.Clock()

    def display(self):
        self._screen.blit(self._background, (0, 0))
        
        #self._handpos = []

        
        ######### hand
        availablecards = len(self._players[0]._hand)
        if(self._status == STATUS_PLAYINTURN): 
            availablecards -= 1
        for i in range(availablecards):
            cd = self._players[0]._hand[i]
            self._screen.blit(cd._img, (HAND_LEFT + i*CARD_WIDTH, HAND_TOP))        
        if(self._status == STATUS_PLAYINTURN): 
            self._screen.blit(self._players[0]._hand[availablecards]._img, (HAND_LEFT + (availablecards + 1) * CARD_WIDTH, HAND_TOP))      
        
        for i in range(len(self._players[0]._ming)):
            cd = (self._players[0]._ming)[-i - 1]
            cd._img = pygame.image.load("./ming/" + cd._text + ".png").convert()
            cd._img = pygame.transform.scale(cd._img, (CARD_WIDTH, CARD_HEIGHT))
            self._screen.blit(cd._img, (HAND_RIGHT - i*CARD_WIDTH, HAND_TOP)) 
        for i in range(len(self._players[0]._gang)):
            cd = (self._players[0]._gang)[i]
            cd._img = pygame.image.load("./ming/" + cd._text + ".png").convert()
            cd._img = pygame.transform.scale(cd._img, (CARD_WIDTH, CARD_HEIGHT))
            self._screen.blit(cd._img, (HAND_RIGHT - i*CARD_WIDTH, HAND_TOP - CARD_HEIGHT))        



        # load history
        #for num in range(0, 4):
        for i in range(len(self._history[0])):
            pos = (390 + i % 8 * 30, 510 + i // 8 * 45)
            image = (self._history[0])[i]._img
            image = pygame.image.load("./ming/" + (self._history[0])[i]._text + '.png').convert()
            image = pygame.transform.scale(image, (30, 45))
            self._screen.blit(image, pos)
        for i in range(len(self._history[1])):
            pos = (640 + i // 8 * 45, 470 - i % 8 * 30)
            image = (self._history[1])[i]._img
            image = pygame.image.load("./right/" + (self._history[1])[i]._text + '.png').convert()
            image = pygame.transform.scale(image, (45, 30))
            self._screen.blit(image, pos)
        for i in range(len(self._history[2])):
            pos = (605 - i % 8 * 30, 205 - i // 8 * 45)
            image = (self._history[2])[i]._img
            image = pygame.image.load("./up/" + (self._history[2])[i]._text + '.png').convert()
            image = pygame.transform.scale(image, (30, 45))
            self._screen.blit(image, pos)
        for i in range(len(self._history[3])):
            pos = (345 - i // 8 * 45, 260 + i % 8 * 30)
            image = (self._history[3])[i]._img
            image = pygame.image.load("./left/" + (self._history[3])[i]._text + '.png').convert()
            image = pygame.transform.scale(image, (45, 30))
            self._screen.blit(image, pos)

        cancelimg = pygame.image.load("cancel.png").convert_alpha()
        cancelimg = pygame.transform.scale(cancelimg, (180, 70))

        if(self._status == STATUS_DECISIONCHI):
            chiimg = pygame.image.load("chi.gif").convert_alpha()
            chiimg = pygame.transform.scale(chiimg, (100, 125))
            self._screen.blit(chiimg, (500, 500))
            self._screen.blit(cancelimg, (700, 530))

        if(self._status == STATUS_DECISIONPENG):
            chiimg = pygame.image.load("peng.gif").convert_alpha()
            chiimg = pygame.transform.scale(chiimg, (100, 125))
            self._screen.blit(chiimg, (500, 500))
            self._screen.blit(cancelimg, (700, 530))

        if(self._status == STATUS_DECISIONGANG):
            chiimg = pygame.image.load("gang.gif").convert_alpha()
            chiimg = pygame.transform.scale(chiimg, (100, 125))
            self._screen.blit(chiimg, (500, 500))
            self._screen.blit(cancelimg, (700, 530))

        if(self._status == STATUS_END):
            ticks = pygame.time.get_ticks()
            self.hugroup.update(ticks)
            self.hugroup.draw(self._screen)

        pygame.display.update()    
            

    def procceed(self):
        self._currplayer += 1
        self._currplayer %= 4

    def draw(self):
        # if empty, end the game; ??????????????????????????????????????????????????
        cardtxt = self._mountain._order[2*self._currnum: 2*self._currnum+2]
        currcard = Card(cardtxt)
        (self._players[self._currplayer]).getcard(currcard)
        # turn()
        self._currnum += 1
        return currcard

    '''  def playcard(self, cd):
        try:
            self._players[self._currplayer]._hand.del(cd)
        except IndexError:
            pass
        
        self._history.append(cd)
        self._players[self._currplayer]._hand.sort()'''

    def turn(self):
        if (self._currplayer != 0):
            self._status = STATUS_DEFAULT
            self.oppoturn()
            self._players[self._currplayer]._hand.sort()
        else:
            #if(self._status != STATUS_DEFAULT):
            #   raise RuntimeError
            self._status = STATUS_PLAYINTURN
            self.playerturn()

        
        self.display()

    def oppoturn(self):
        self.draw()
        self.display()
        pygame.time.wait(random.randint(100, 1500))
        if(self._currplayer == 0):
            raise RuntimeError ###############
        cd = self._players[self._currplayer]._hand[random.randint(0, 13)]
        self._players[self._currplayer].playcard(cd)
        self._history[self._currplayer].append(cd)
        self.checkhu()
        self.checkgang()
        self.checkpeng()
        if(self._currplayer == 3):
            self.checkchi()
        self.procceed()

    def playerturn(self):
        #self._screen.blit(self.draw()._img, (900, 650))
        #pygame.display.update()
        self.draw()
        self.checkhu()
        self._status = STATUS_PLAYINTURN

    def checkhu(self):
        yourhand = list(self._players[0]._hand)
        strarr = [r._text for r in yourhand]
        yourming = list(self._players[0]._ming)
        for _ in range(0, len(yourming) // 3):
            for i in range(1, 8):
                if(strarr.count(str(i) + 'z') == 0):
                    yourhand.append(Card(str(i) + 'z'))
                    yourhand.append(Card(str(i) + 'z'))
                    yourhand.append(Card(str(i) + 'z'))
                    strarr = strarr + [str(i) + 'z', str(i) + 'z', str(i) + 'z']
                    break
        hand = yourhand
        if(self._currplayer != 0):
            hand = hand + [self._history[self._currplayer][-1]]
        analy = cal_win.Analyser(hand)
        ans = analy.get_res()
        if (ans == -1):
            return
        else:
            raise HUError

    def checkchi(self):
        cd = (self._history[self._currplayer])[-1]
        if(cd._attr != 'z'):
            yourhand = self._players[(self._currplayer + 1) % 4]._hand
            strarr = [r._text for r in yourhand]
            choices = []
            if((str(cd._num-2) + cd._attr) in strarr and (str(cd._num-1) + cd._attr) in strarr):
                choices.append((yourhand[strarr.index(str(cd._num-2) + cd._attr)], yourhand[strarr.index(str(cd._num-1) + cd._attr)]))
            if((str(cd._num-1) + cd._attr) in strarr and (str(cd._num+1) + cd._attr) in strarr):
                choices.append((yourhand[strarr.index(str(cd._num-1) + cd._attr)], yourhand[strarr.index(str(cd._num+1) + cd._attr)]))
            if((str(cd._num+1) + cd._attr) in strarr and (str(cd._num+2) + cd._attr) in strarr):
                choices.append((yourhand[strarr.index(str(cd._num+1) + cd._attr)], yourhand[strarr.index(str(cd._num+2) + cd._attr)]))
            self.chichoices = choices
            if(len(choices) != 0):
                raise CHIError
        return False

    def checkpeng(self):
        cd = (self._history[self._currplayer])[-1]
        yourhand = list(self._players[0]._hand)   
        strarr = [r._text for r in yourhand]
        if(strarr.count(cd._text) >= 2):
            self.pengindex = strarr.index(cd._text)
            raise PENGError         
        return False

    def checkgang(self):
        cd = (self._history[self._currplayer])[-1]
        if(self._currplayer == 0):
            cd = self._players[0]._hand[-1]
        yourhand = list(self._players[0]._hand)
        yourming = list(self._players[0]._ming)   
        strarr1 = [r._text for r in yourhand]
        strarr2 = [r._text for r in yourming]
        if(strarr1.count(cd._text) >= 3):
            self.gangindex = strarr1.index(cd._text)
            if(strarr1[self.gangindex + 2] == cd.text):
                raise GANGError
        if(self._currplayer == 0 and strarr2.count(cd._text) >= 3):
            self._players[0]._gang.append(cd)
            self.playerturn()
        return False

    def start(self):
        self._clock.tick(20)
        for _ in range(0, 52):
            self.draw()
            self.procceed()
        for pl in self._players:
            pl._hand.sort()
        for i in range(len(pl._hand)):
            cd = pl._hand[i]
            self._screen.blit(cd._img, (HAND_LEFT + i*CARD_WIDTH, HAND_TOP))
            pygame.display.update()
            pygame.time.wait(70)

    def run(self):
        self.start()
        self.turn()
        while(1):
            if(self._status != STATUS_CHOICECHI2):
                self.display()
            for event in pygame.event.get():   
            #event = pygame.event.wait()
                try:
                #####
                    if (event.type == QUIT):
                        sys.exit(0)
                    if (event.type == pygame.MOUSEBUTTONDOWN):
                        #raise HUError
                        if (self._status == STATUS_DEFAULT):
                            continue
                        if (self._status == STATUS_PLAYINTURN):
                            (cursorX, cursorY) = pygame.mouse.get_pos()
                            availablecards = 13 - self._players[0]._nummingcards
                            if  (cursorY in range(HAND_TOP, HAND_TOP + CARD_HEIGHT) and
                                    (cursorX in range(HAND_LEFT, HAND_LEFT + availablecards*CARD_WIDTH) or
                                    cursorX in range(HAND_LEFT + (availablecards + 1)*CARD_WIDTH, HAND_LEFT + (availablecards + 2)*CARD_WIDTH)
                                    )
                                ):
                                indx = (cursorX - HAND_LEFT) // CARD_WIDTH
                                if indx == availablecards + 1 : indx -= 1
                                cd = self._players[0]._hand[indx]
                                self._players[0].playcard(cd)
                                self._history[0].append(cd)
                                self._players[self._currplayer]._hand.sort()
                                self.procceed()
                                self._status = STATUS_DEFAULT
                                self.turn()
                                self.turn()
                                self.turn()
                                self.turn()
                        if (self._status == STATUS_DECISIONCHI):
                            (cursorX, cursorY) = pygame.mouse.get_pos()
                            if  (cursorY in range(500, 625) and cursorX in range(500, 600)):
                                cd = (self._history[self._currplayer])[-1]
                                if(len(self.chichoices) == 1):
                                    (cd1, cd2) = (self.chichoices[0])
                                    self._history[self._currplayer].remove(cd)
                                    self._players[0]._hand.remove(cd1)
                                    self._players[0]._hand.remove(cd2)
                                    self._players[0].mingcard((cd, cd1, cd2))
                                    self._status = STATUS_PLAYEXTURN
                                elif(len(self.chichoices) == 2):
                                    self._status = STATUS_CHOICECHI2
                                    self.display()
                                    (cd1, cd2) = (self.chichoices[0])
                                    self._screen.blit(cd1._img, (320, 500))
                                    self._screen.blit(cd2._img, (360, 500))
                                    (cd3, cd4) = (self.chichoices[1])
                                    self._screen.blit(cd3._img, (600, 500))
                                    self._screen.blit(cd4._img, (640, 500))
                                    pygame.display.update()
                                elif(len(self.chichoices) == 3):
                                    self._status = STATUS_CHOICECHI3
                                    self.display()
                                    (cd1, cd2) = (self.chichoices[0])
                                    self._screen.blit(cd2._img, (320, 500))
                                    self._screen.blit(cd1._img, (360, 500))
                                    (cd3, cd4) = (self.chichoices[1])
                                    self._screen.blit(cd3._img, (460, 500))
                                    self._screen.blit(cd4._img, (500, 500))
                                    (cd5, cd6) = (self.chichoices[2])
                                    self._screen.blit(cd5._img, (600, 500))
                                    self._screen.blit(cd6._img, (640, 500))
                                    pygame.display.update()
                            elif(cursorY in range(530, 600) and cursorX in range(700, 880)):
                                self._status = STATUS_DEFAULT
                                self.procceed()
                                self.turn()
                        
                        if (self._status == STATUS_CHOICECHI2):
                            (cursorX, cursorY) = pygame.mouse.get_pos()
                            cd = (self._history[self._currplayer])[-1] 
                            cd1, cd2 = None, None
                            if  (cursorY in range(500, 560) and cursorX in range(320, 400)):
                                (cd1, cd2) = (self.chichoices[0])
                            elif(cursorY in range(500, 560) and cursorX in range(600, 680)):
                                (cd1, cd2) = (self.chichoices[1])
                            if(cd1 == None): continue
                            self._history[self._currplayer].remove(cd)
                            self._players[0]._hand.remove(cd1)
                            self._players[0]._hand.remove(cd2)
                            self._players[0].mingcard((cd, cd1, cd2))
                            self._status = STATUS_PLAYEXTURN

                        if (self._status == STATUS_CHOICECHI3):
                            (cursorX, cursorY) = pygame.mouse.get_pos()
                            cd = (self._history[self._currplayer])[-1]
                            cd1, cd2 = None, None
                            if  (cursorY in range(500, 560) and cursorX in range(320, 400)):
                                (cd1, cd2) = (self.chichoices[0])
                            elif(cursorY in range(500, 560) and cursorX in range(460, 540)):
                                (cd1, cd2) = (self.chichoices[1])
                            elif(cursorY in range(500, 560) and cursorX in range(600, 680)):
                                (cd1, cd2) = (self.chichoices[2])
                            if(cd1 == None): continue
                            self._history[self._currplayer].remove(cd)
                            self._players[0]._hand.remove(cd1)
                            self._players[0]._hand.remove(cd2)
                            self._players[0].mingcard((cd, cd1, cd2))
                            self._status = STATUS_PLAYEXTURN    

                        if (self._status == STATUS_DECISIONPENG):
                            (cursorX, cursorY) = pygame.mouse.get_pos()
                            if  (cursorY in range(500, 625) and cursorX in range(500, 600)):
                                cd = (self._history[self._currplayer])[-1]
                                (cd1, cd2) = (self._players[0]._hand[self.pengindex], self._players[0]._hand[self.pengindex + 1])
                                self._history[self._currplayer].remove(cd)
                                self._players[0]._hand.remove(cd1)
                                self._players[0]._hand.remove(cd2)
                                self._players[0].mingcard((cd, cd1, cd2))
                                self._status = STATUS_PLAYEXTURN
                            elif(cursorY in range(530, 600) and cursorX in range(700, 880)):
                                self._status = STATUS_DEFAULT
                                self.procceed()
                                while(self._currplayer != 1):
                                    self.turn()

                        if (self._status == STATUS_DECISIONGANG):
                            (cursorX, cursorY) = pygame.mouse.get_pos()
                            if  (cursorY in range(500, 625) and cursorX in range(500, 600)):
                                cd = (self._history[self._currplayer])[-1]
                                (cd1, cd2, cd3) = (self._players[0]._hand[self.gangindex], self._players[0]._hand[self.gangindex+1], self._players[0]._hand[self.gangindex+2])
                                self._history[self._currplayer].remove(cd)
                                self._players[0]._hand.remove(cd1)
                                self._players[0]._hand.remove(cd2)
                                self._players[0]._hand.remove(cd3)
                                self._players[0].mingcard((cd1, cd2, cd3))
                                self._players[0]._gang.append(cd)
                                self._currplayer = 0
                                self._status = STATUS_PLAYINTURN
                                self.playerturn()
                            elif(cursorY in range(530, 600) and cursorX in range(700, 880)):
                                self._status = STATUS_DEFAULT
                                self.procceed()
                                while(self._currplayer != 1):
                                    self.turn()


                        if (self._status == STATUS_PLAYEXTURN):
                            (cursorX, cursorY) = pygame.mouse.get_pos()
                            availablecards = 14 - self._players[0]._nummingcards
                            if  (cursorY in range(HAND_TOP, HAND_TOP + CARD_HEIGHT) and
                                    cursorX in range(HAND_LEFT, HAND_LEFT + availablecards*CARD_WIDTH)
                                ):
                                indx = (cursorX - HAND_LEFT) // CARD_WIDTH
                                cd = self._players[0]._hand[indx]
                                self._players[0].playcard(cd)
                                self._history[0].append(cd)
                                self._currplayer = 0
                                self._players[self._currplayer]._hand.sort()
                                self.procceed()
                                self._status = STATUS_DEFAULT
                                self.turn()
                                self.turn()
                                self.turn()
                                self.turn()
                except HUError:
                    self._status = STATUS_END
                    self.hugif = animation.MySprite(self._screen)
                    self.hugif.load("hu.png", 265.5, 150, 4)
                    self.hugroup = pygame.sprite.Group()
                    self.hugroup.add(self.hugif)
                    self._clock.tick(30)
                except GANGError:
                    self._status = STATUS_DECISIONGANG
                except PENGError:
                    self._status = STATUS_DECISIONPENG
                except CHIError:
                    self._status = STATUS_DECISIONCHI
                except EmptyError:
                    pass



def main():
    p1 = Player('A', '', '', '')
    # contract_interface_p1 = deploy.create_contract()
    # deploy.deploy(contract_interface_p1, p1.path, p1.pwd, p1.addr)
    p2 = Player('B', '', '', '')
    # contract_interface_p2 = deploy.create_contract()
    # deploy.deploy(contract_interface_p2, p2.path, p2.pwd, p2.addr)
    p3 = Player('C', '', '', '')
    # contract_interface_p3 = deploy.create_contract()
    # deploy.deploy(contract_interface_p3, p3.path, p3.pwd, p3.addr)
    p4 = Player('D', '', '', '')
    # contract_interface_p4 = deploy.create_contract()
    # deploy.deploy(contract_interface_p4, p4.path, p4.pwd, p4.addr)

    dbt = Game(p1, p2, p3, p4)
    #print(dbt._mountain._content)
    dbt.run()

if __name__ == "__main__":
    main()







