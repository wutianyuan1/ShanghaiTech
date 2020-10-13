import random
import numpy as np
# coin that lands head with prob=a_{ij}
def coin(p):
    flip=random.random()
    if flip < p:
        return 1
    return 0
    
def ifindep(setX,alledge,choosev):
    relatev=[]
    for e in alledge:
        if e[0]==choosev:
            relatev.append(e[1])
        elif e[1]==choosev:
            relatev.append(e[0])
    ind=1
    for e in relatev:
        if e in setX:
            ind=0
    return ind

def Discrete_MH(setX,alledge,lam):
    iters=1000
    for t in range(iters):
        choosev=np.random.randint(1,24)
        if choosev in setX:
            aij=min(1,1/lam)
            if coin(aij)==1:
                setX.remove(choosev)
        else:
            if ifindep(setX,alledge,choosev)==1:
                aij=min(1,lam)
                if coin(aij)==1:
                    setX.append(choosev)
    return setX,len(setX)



lam=0.5
vertice_num=24
edge_num=36
alledge=[]
for i in range(12):
    alledge.append([i+1,i+13])
for i in range(11):
    alledge.append([i+13,i+14])
alledge.append([24,13])
for i in range(12):
    endp=(i+5)%12
    if endp==0:
        endp=12
    alledge.append([i+1,endp])
print('edge num',len(alledge))
X0=[1]
allsize=[]
iters=2000
for t in range(iters):
    setX,setsize=Discrete_MH(X0,alledge,lam)
    allsize.append(setsize)
    # print(setX,setsize)
print('Discrete\tsize of maximum independent set is ',max(allsize))
