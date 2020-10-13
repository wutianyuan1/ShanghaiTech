import random
import sys
import math

def distance_point(p1,p2):
    counter = 0
    for i in range(len(p1)):
        square_dis = (p1[i]-p2[i])**2
        counter += square_dis
    counter = (counter)**0.5
    return counter

def vector(p1,p2):
    vec_p1_p2 = []
    for i in range(len(p1)):
        vec_p1_p2.append((p2[i]-p1[i]))
    return vec_p1_p2

def distance_pline(A,B,M):
    dot = 0
    AM = vector(A,M)
    AB = vector(A,B)
    for i in range(len(A)):
        dot += (AM[i]*AB[i])
    cos_theta = dot/(distance_point(A,M)*distance_point(A,B))
    sin_theta = (1-(cos_theta)**2)**0.5
    h = distance_point(A,M)*sin_theta
    return h

def main():
    total = []
    for line in sys.stdin:
        total.append(line.split())
    n = float(total[0][0]) #要找几条直线
    eps = float(total[0][1]) #离得多近才算内点
    T = float(total[0][2]) #最大循环次数
    p = float(total[0][3]) #俩都是内点的概率
    w = float(total[0][4]) #内点占总数的比例
    pointset = []
    for i in range(1,len(total)):
        for j in range(len(total[i])):
            total[i][j] = float(total[i][j])
        pointset.append(total[i])#到此为止 把所有点放进了pointset，设置了所有参数
    abc = []
    if p == 0 or w == 0 :
        n0 = T
    else :
        k = math.log(1 - p)/math.log(1 - w**2)
        n0 = min(k,T)#n0为找直线时ransac迭代的次数

    for t in range(int(n)):
        totalin = 0
        shuchu = []
        zhenyaode = []
        for x in range(int(n0)):
            linepoint = random.sample(pointset,2)
            A = linepoint[0]
            B = linepoint[1]    
            dset = []
            for i in range(len(pointset)):
                d = distance_pline(A,B,pointset[i])
                dset.append([pointset[i],d])#dset为所有点到刚刚确立的直线的距离的组合
            inliers = 0
            yaode = []
            for i in range(len(dset)):
                if dset[i][1] <= eps:
                    inliers = inliers +1
                    yaode.append(dset[i][0])
                else:
                    inliers = inliers #数数有几个inliers        
            if inliers >= totalin:
                totalin = inliers
                zhenyaode = yaode
                shuchu = [A,B]
        abc.append(shuchu)
        for i in range(len(pointset)):
            pointset[i] = tuple(pointset[i])
        for j in range(len(zhenyaode)):
            zhenyaode[j] = tuple(zhenyaode[j])
        pointset = list(set(pointset) - set(zhenyaode))
        for k in range(len(pointset)):
            pointset[k] = list(pointset[k])
        return()


for t0 in range(len(abc)):
    if abc[t0][0] < 0:
        abc[t0][0] = - abc[t0][0]
        abc[t0][1] = - abc[t0][1]
        abc[t0][2] = - abc[t0][2]
    else :
        abc[t0][0] = abc[t0][0]
        abc[t0][1] = abc[t0][1]
        abc[t0][2] = abc[t0][2]
for i in range(len(abc)):
    x0 = ((abc[i][0])**2+(abc[i][1])**2+(abc[i][2])**2)**0.5
    abc[i][0] =  round(abc[i][0]/x0,8)
    abc[i][1] =  round(abc[i][1]/x0,8)
    abc[i][2] =  round(abc[i][2]/x0,8)
abc = sorted(abc)
for mm in range(len(abc)):
    print("%.8f" %(abc[mm][0]), end = " ")
    print("%.8f" %(abc[mm][1]), end = " ")
    print("%.8f" %(abc[mm][2]))