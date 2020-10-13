import numpy as np
import matplotlib.pyplot as plt
import sys
#if your computer doesn't have matplotlib,you should install it on cmd by using following code.
#python -mpip install -U matplotlib
#Assuming near plane is the image plane
class Camera:
    def __init__(self,fov,focal_length,far,near,left,right,bottom,top):
        self.fov=fov
        self.focal_length=focal_length
        self.far=far
        self.near=near
        self.left=left
        self.right=right
        self.bottom=bottom
        self.top=top

def projection(x_3d,camera):
    nn = camera.near
    x = -1*x_3d[0]*(nn/x_3d[2])
    y = -1*x_3d[1]*(nn/x_3d[2])
    x_2d = [x,y,1]
    return(x_2d)

def clipping_3d(x1,x2,camera):
    after = []
    n = camera.near
    f = camera.far
    b = camera.bottom
    t = camera.top
    r = camera.right
    l = camera.left
    zzn = x1[0,2]/x1[0,3]
    znn = x2[0,2]/x2[0,3]
    if zzn>znn>-n or znn>zzn>-n or zzn<znn<-f or znn<zzn<-f:
        return(None)
    else:
        m = np.mat([[2*n/(r-l),0,(r+l)/(r-l),0],[0,2*n/(t-b),(t+b)/(t-b),0],[0,0,(f+n)/(n-f),2*f*n/(n-f)],[0,0,-1,0]])
        n1 = m.I
        A = np.mat(x1).T
        B = np.mat(x2).T
        AA = np.dot(m,A).T
        BB = np.dot(m,B).T
        diyige = []
        dierge = []
        for j in range(3):
            q = AA[0,j]/AA[0,3]
            qq = BB[0,j]/BB[0,3]
            diyige.append(q)
            dierge.append(qq)
        zong = [diyige,dierge]
        x1 = zong[0][0]
        x2 = zong[1][0]
        y1 = zong[0][1]
        y2 = zong[1][1]
        z1 = zong[0][2]
        z2 = zong[1][2]
        l1 = []
        if y2>y1>1 or y1>y2>1 or y1<y2<-1 or y2<y1<-1:
            return(None)
        elif x2>x1>1 or x1>x2>1 or x1<x2<-1 or x2<x1<-1:
            return(None)
        else:
            if x1 != x2 :
                if -1<((1-x1)*(y2-y1)/(x2-x1))+y1<=1 and -1<((1-x1)*(z2-z1)/(x2-x1))+z1<=1:
                    l1.append([1.0,round(((1-x1)*(y2-y1)/(x2-x1))+y1,4),round(((1-x1)*(z2-z1)/(x2-x1))+z1,4)])
                if -1<=((-1-x1)*(y2-y1)/(x2-x1))+y1<=1 and -1<=((-1-x1)*(z2-z1)/(x2-x1))+z1<=1:
                    l1.append([-1.0,round(((-1-x1)*(y2-y1)/(x2-x1))+y1,4),round(((-1-x1)*(z2-z1)/(x2-x1))+z1,4)])
            if y1 != y2 :
                if -1<=((1-y1)*(x2-x1)/(y2-y1))+x1<=1 and -1<=((1-y1)*(z2-z1)/(y2-y1))+z1<=1:
                    l1.append([round(((1-y1)*(x2-x1)/(y2-y1))+x1,4),1.0,round(((1-y1)*(z2-z1)/(y2-y1))+z1,4)])
                if -1<=((-1-y1)*(x2-x1)/(y2-y1))+x1<=1 and -1<=((-1-y1)*(z2-z1)/(y2-y1))+z1<=1:
                    l1.append([round(((-1-y1)*(x2-x1)/(y2-y1))+x1,4),-1.0,round(((-1-y1)*(z2-z1)/(y2-y1))+z1,4)])
            if z1 != z2 :
                if -1<=((1-z1)*(x2-x1)/(z2-z1))+x1<=1 and -1<((1-z1)*(y2-y1)/(z2-z1))+y1<=1:
                    l1.append([round(((1-z1)*(x2-x1)/(z2-z1))+x1,4),round(((1-z1)*(y2-y1)/(z2-z1))+y1,4),1.0])
                if -1<=((-1-z1)*(x2-x1)/(z2-z1))+x1<=1 and -1<=((-1-z1)*(y2-y1)/(z2-z1))+y1<=1:   
                    l1.append([round(((-1-z1)*(x2-x1)/(z2-z1))+x1,4),round(((-1-z1)*(y2-y1)/(z2-z1))+y1,4),-1.0])
            for i in range(len(l1)):
                l1[i] = tuple(l1[i])
            l1 = list(set(l1))
            for i in range(len(l1)):
                l1[i] = list(l1[i])
            if len(l1) == 2:
                l1.append([round(x1,4),round(y1,4),round(z1,4)])
                l1.append([round(x2,4),round(y2,4),round(z2,4)])
                l1 = sorted(l1)
                after.append([l1[1],l1[2]])
            if after != []:
                bb = after[0][0]
                ss = after[0][1]
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
                pset = [zs,zx]
                return(pset)
        #x1 is numpy arrary in homogeneous [x,y,z,1]

def clipping_3d1(x1,x2,camera):
    n = camera.near
    f = camera.far
    zz1 = x1[0,2]/x1[0,3]
    zz2 = x2[0,2]/x2[0,3]
    xx1 = x1[0,0]/x1[0,3]
    xx2 = x2[0,0]/x2[0,3]
    yy1 = x1[0,1]/x1[0,3]
    yy2 = x2[0,1]/x2[0,3]
    if -f < zz1 < -n and -f < zz2 < -n:
        x01 = [x1[0,0],x1[0,1],x1[0,2]]
        x02 = [x2[0,0],x2[0,1],x2[0,2]]
        return([x01,x02])
    elif -f < zz1 < -n and zz2 > -n :
        l1 = []
        l1.append([round(((-1-zz1)*(xx2-xx1)/(zz2-zz1))+xx1,4),round(((-1-zz1)*(yy2-yy1)/(zz2-zz1))+yy1,4),-1.0])
        l1.append([xx1,yy1,zz1])
        return(l1)
    elif -f < zz2 < -n and zz1 > -n :
        l1 = []
        l1.append([round(((-1-zz1)*(xx2-xx1)/(zz2-zz1))+xx1,4),round(((-1-zz1)*(yy2-yy1)/(zz2-zz1))+yy1,4),-1.0])
        l1.append([xx2,yy2,zz2])
        return(l1)
    elif -f < zz1 < -n and zz2 < -f :
        l1 = []
        l1.append([round(((-10-zz1)*(xx2-xx1)/(zz2-zz1))+xx1,4),round(((-10-zz1)*(yy2-yy1)/(zz2-zz1))+yy1,4),-10])
        l1.append([xx1,yy1,zz1])
        return(l1)
    elif -f < zz2 < -n and zz1 < -f :
        l1 = []
        l1.append([round(((-10-zz1)*(xx2-xx1)/(zz2-zz1))+xx1,4),round(((-10-zz1)*(yy2-yy1)/(zz2-zz1))+yy1,4),-10])
        l1.append([xx2,yy2,zz2])
        return(l1)
    elif (zz1 <-f and zz2>-n) or (zz2<-f and zz1>-n):
        l1 = []
        l1.append([round(((-10-zz1)*(xx2-xx1)/(zz2-zz1))+xx1,4),round(((-10-zz1)*(yy2-yy1)/(zz2-zz1))+yy1,4),-10])
        l1.append([round(((-1-zz1)*(xx2-xx1)/(zz2-zz1))+xx1,4),round(((-1-zz1)*(yy2-yy1)/(zz2-zz1))+yy1,4),-1.0])
        return(l1)
    else:
        return(None)

def encode(x1):
    #x is 2d point 
    #output binary embedding code
    if(x1[0]<=1 and x1[0]>=-1 and x1[1]<=1 and x1[1]>=-1):
        return int('0000',2)
    if((x1[0]>=1 and x1[1]>=1)):
        return int('1010',2)
    if((x1[0]>-1 and x1[0]<1) and x1[1]>1):
        return int('1000',2)
    if(x1[0]<=-1 and x1[1]>=1):
        return int('1001',2)
    if(x1[0]<-1 and (x1[1]>-1 and x1[1]<1)):
        return int('0001',2)
    if(x1[0]<=-1 and x1[1]<=-1):
        return int('0101',2)
    if(x1[0]>-1 and x1[0]<1 and x1[1]<-1):
        return int('0100',2)
    if(x1[0]>=1 and x1[1]<=-1):
        return int('0110',2)
    if(x1[0]>1 and (x1[1]>-1 and x1[1]<1)):
        return int('0010',2)

def clipping_2d(x1,x2,camera):
    #x1,x2 are 2d points in homogeneous coordinate e.g. [x,y,1]
    b = camera.bottom
    t = camera.top
    r = camera.right
    l = camera.left
    xx1 = x1[0]/x1[2]
    yy1 = x1[1]/x1[2]
    xx2 = x2[0]/x2[2]
    yy2 = x2[1]/x2[2]
    p1 = encode([xx1,yy1])
    p2 = encode([xx2,yy2])
    if p1 | p2 == 0:
        return([[xx1,yy1],[xx2,yy2]])
    if p1 & p2 != 0 :
        return(None)
    else:
        
        B = xx2 - xx1
        A = yy1 - yy2
        C = xx1*yy2 - xx2*yy1
        kong = []
        if xx1 != xx2:
            if -1 <= -(A*r+C)/B <= 1:
                kong.append([r,round((-(A*r+C)/B),4)])
            if -1 <= -(A*l+C)/B <= 1:
                kong.append([l,round((-(A*l+C)/B),4)])
        if yy1 != yy2:
            if -1 <= -(B*t+C)/A <= 1:
                kong.append([round((-(B*t+C)/A),4),t])
            if -1 <= -(B*b+C)/A <= 1:
                kong.append([round((-(B*b+C)/A),4),b])
        for i in range(len(kong)):
            kong[i] = tuple(kong[i])
        kong = list(set(kong))
        for i in range(len(kong)):
            kong[i] = list(kong[i])
        if len(kong) == 2:
            kong.append([round(xx1,4),round(yy1,4)])
            kong.append([round(xx2,4),round(yy2,4)])
            kong = sorted(kong)
            kong[1].append(1)
            kong[2].append(1)
            afterr = [kong[1],kong[2]]
        else:
            afterr = None
        return(afterr)
            
def rasterization_dda(x1, x2, camera,w,h):
    A0 = x1
    B0 = x2
    xx1 = w*(1+A0[0])/2
    xx2 = w*(1+B0[0])/2
    yy1 = h-(h*(1-A0[1])/2)
    yy2 = h-(h*(1-B0[1])/2)
    A = [xx1,yy1]
    B = [xx2,yy2]
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
            xw = int(round(A[0]+i*dx))
            yw = int(round(A[1]+i*dy))
            plt.scatter(round(xw), round(yw), c='r')
    else:
        k1 = (A[0]-B[0])/(A[1]-B[1])
        if A[1] < B[1]:
            dy = 1
            dx = k1*dy
        if A[1] > B[1]:
            dy = -1
            dx = k1*dy
        for j in range(int(abs(B[1]-A[1])+1)):
            xz = int(round(A[0]+j*dx))
            yz = int(round(A[1]+j*dy))
            plt.scatter(round(xz), round(yz), c='r')
    plt.axis([0,w,0,h])
    frame=plt.gca()
    frame.axes.get_yaxis().set_visible(False)
    frame.axes.get_xaxis().set_visible(False)
    plt.show()

if __name__ == '__main__':
    w=320 # w (width) and h(height) are the window size.
    h=240
    cam=Camera(fov=90,focal_length=1,far=10,near=1,left=-1,right=1,bottom=-1,top=1)
    mat = []
    for line in sys.stdin:
        mat.append(line.split())
    wsx = []
    for i in range(len(mat)):
        pp = []
        py = []
        for j in range(3):
            pp.append(float(mat[i][j]))
        pp.append(1)
        for k in range(3,6):
            py.append(float(mat[i][k]))
        py.append(1)
        wsx.append([pp,py])
    for i in range(len(wsx)):
        ass = np.mat(wsx[i][0])
        ssa = np.mat(wsx[i][1])
        www = clipping_3d1(ass,ssa,cam)
        if www != None :
            ddd1 = www[0]
            ddd2 = www[1]
            ddd1.append(1)
            ddd2.append(1)
            xy1 = projection(ddd1,cam)
            xy2 = projection(ddd2,cam)
            listt = clipping_2d(xy1,xy2,cam)
            if listt != None :
                xy1 = listt[0]
                xy2 = listt[1]
                rasterization_dda(xy1,xy2,cam,w,h)
            else:
                xy1 = [2,2,1]
                xy2 = [2,2.01,1]
                rasterization_dda(xy1,xy2,cam,w,h)
        else :
            xy1 = [2,2,1]
            xy2 = [2,2.01,1]
            rasterization_dda(xy1,xy2,cam,w,h)

