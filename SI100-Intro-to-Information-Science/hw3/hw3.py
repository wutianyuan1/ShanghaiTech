import numpy as np
import sys
import matplotlib.pyplot as plt
from PIL import Image
mat = []
for line in sys.stdin:
    mat.append(line.split())
n = float(mat[0][0])   #近平面
f = float(mat[0][1])   #远平面
h = float(mat[0][2])   #相纸高度
w = float(mat[0][3])   #相纸宽度
l = -w/2
r = w/2
t = h/2
b = -h/2
m = np.mat([[2*n/(r-l),0,(r+l)/(r-l),0],[0,2*n/(t-b),(t+b)/(t-b),0],[0,0,(f+n)/(n-f),2*f*n/(n-f)],[0,0,-1,0]])#您要的transition matrix
n1 = m.I#您要的逆矩阵

pointset = []
for i in range(1,len(mat)):
    pp = []
    py = []
    for j in range(3):
        pp.append(float(mat[i][j]))
    pp.append(1)
    for k in range(3,6):
        py.append(float(mat[i][k]))
    py.append(1)
    pointset.append([pp,py])

zong = []
for i in range(len(pointset)):
    A = np.mat(pointset[i][0]).T
    B = np.mat(pointset[i][1]).T
    AA = np.dot(m,A).T
    BB = np.dot(m,B).T
    diyige = []
    dierge = []
    for j in range(3):
        q = AA[0,j]/AA[0,3]
        qq = BB[0,j]/BB[0,3]
        diyige.append(q)
        dierge.append(qq)
    zong.append([diyige,dierge]) #至此为止，总的（zong）里面是[[(x1,y1,z1),(x2,y2,z2)],[(),()]……]这样，这些坐标都是单位正方体里面的坐标了。


after = []#在单位正方体里面执行裁剪 输出到after里面
for i in range(len(zong)):
    x1 = zong[i][0][0]
    x2 = zong[i][1][0]
    y1 = zong[i][0][1]
    y2 = zong[i][1][1]
    z1 = zong[i][0][2]
    z2 = zong[i][1][2]
    l1 = []
    if x1 != x2 :
        if -1<((1-x1)*(y2-y1)/(x2-x1))+y1<=1 and -1<((1-x1)*(z2-z1)/(x2-x1))+z1<=1:
            l1.append([1.0,round(((1-x1)*(y2-y1)/(x2-x1))+y1,2),round(((1-x1)*(z2-z1)/(x2-x1))+z1,2)])
        if -1<=((-1-x1)*(y2-y1)/(x2-x1))+y1<=1 and -1<=((-1-x1)*(z2-z1)/(x2-x1))+z1<=1:
            l1.append([-1.0,round(((-1-x1)*(y2-y1)/(x2-x1))+y1,2),round(((-1-x1)*(z2-z1)/(x2-x1))+z1,2)])
    if y1 != y2 :
        if -1<=((1-y1)*(x2-x1)/(y2-y1))+x1<=1 and -1<=((1-y1)*(z2-z1)/(y2-y1))+z1<=1:
            l1.append([round(((1-y1)*(x2-x1)/(y2-y1))+x1,2),1.0,round(((1-y1)*(z2-z1)/(y2-y1))+z1,2)])
        if -1<=((-1-y1)*(x2-x1)/(y2-y1))+x1<=1 and -1<=((-1-y1)*(z2-z1)/(y2-y1))+z1<=1:
            l1.append([round(((-1-y1)*(x2-x1)/(y2-y1))+x1,2),-1.0,round(((-1-y1)*(z2-z1)/(y2-y1))+z1,2)])
    if z1 != z2 :
        if -1<=((1-z1)*(x2-x1)/(z2-z1))+x1<=1 and -1<((1-z1)*(y2-y1)/(z2-z1))+y1<=1:
            l1.append([round(((1-z1)*(x2-x1)/(z2-z1))+x1,2),round(((1-z1)*(y2-y1)/(z2-z1))+y1,2),1.0])
        if -1<=((-1-z1)*(x2-x1)/(z2-z1))+x1<=1 and -1<=((-1-z1)*(y2-y1)/(z2-z1))+y1<=1:   
            l1.append([round(((-1-z1)*(x2-x1)/(z2-z1))+x1,2),round(((-1-z1)*(y2-y1)/(z2-z1))+y1,2),-1.0])
    for i in range(len(l1)):
        l1[i] = tuple(l1[i])
    l1 = list(set(l1))
    for i in range(len(l1)):
        l1[i] = list(l1[i])
    if len(l1) == 2:
        l1.append([x1,y1,z1])
        l1.append([x2,y2,z2])
        l1 = sorted(l1)
        after.append([l1[1],l1[2]])

pset = []
for i in range(len(after)):
    bb = after[i][0]
    ss = after[i][1]
    ww = []
    cc = []
    for j in range(len(bb)):
        ww.append(bb[j])
    ww.append(1)
    for k in range(len(ss)):
        cc.append(ss[k])
    cc.append(1)
    woc = np.mat(ww).T
    wco = np.mat(cc).T
    nb = np.dot(n1,woc).T
    sb = np.dot(n1,wco).T
    zs = []
    zx = []
    for s in range(3):
        zx.append(nb[0,s]/nb[0,3])
        zs.append(sb[0,s]/sb[0,3])
    pset.append([zs,zx]) #pset是裁剪好的坐标

ty = []
for i in range(len(pset)):
    xx1 = -1*pset[i][0][0]*n/pset[i][0][2]
    xx2 = -1*pset[i][1][0]*n/pset[i][1][2]
    yy1 = -1*pset[i][0][1]*n/pset[i][0][2]
    yy2 = -1*pset[i][1][1]*n/pset[i][1][2]
    t1 = [[xx1,yy1],[xx2,yy2]]
    ty.append(t1)#投影坐标

for i in range(len(ty)):
    ty[i][0][0] = ty[i][0][0] + w/2
    ty[i][1][0] = ty[i][1][0] + w/2
    ty[i][0][1] = ty[i][0][1] + h/2
    ty[i][1][1] = ty[i][1][1] + h/2#坐标变换
 #ty是投影好等待画线的坐标点。
img = Image.new('RGB',(int(w+1),int(h+1)))
a = np.array(img)

for i in range(len(a)):
    for j in range(len(a[1,:,:])):
        a[i,j,:] = (255,255,255)

for i in range(len(ty)):
    A = ty[i][0]
    B = ty[i][1]
    if (A[0]-B[0]) != 0 :
        k = (A[1]-B[1])/(A[0]-B[0])
    else :
        k = 2

    if abs(k) < 1 :
        if A[0] < B[0]:
            dx = 1
            dy = k*dx
        if A[0] > B[0]:
            dx = -1
            dy = k*dx
        for i in range(int(abs(B[0]-A[0])+1)):
            xw = int(round(A[0]+i*dx)) - int(w)
            yw = int(h) - int(round(A[1]+i*dy))
            a[yw,xw,:] = (39,197,187)

    else:
        k1 = (A[0]-B[0])/(A[1]-B[1])
        if A[1] < B[1]:
            dy = 1
            dx = k1*dy
        if A[1] > B[1]:
            dy = -1
            dx = k1*dy
        for j in range(int(abs(B[1]-A[1])+1)):
            xz = int(w) - int(round(A[0]+j*dx))
            yz = int(h) - int(round(A[1]+j*dy))
            a[yz,xz,:] = (39,197,187)

plt.figure("fucker")
plt.imshow(a)
plt.show()
