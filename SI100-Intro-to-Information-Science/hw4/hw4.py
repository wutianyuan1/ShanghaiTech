import sys
def readlines(): #readlines is a function which is used to read the map
    mat = []#at first, mat is empty
    for line in sys.stdin:#sys.stdin can read the map
        mat.append(line.split())   
    return(mat)#return map can make the main program use the data

def yaode(mat): #this is a function used to give the right formula of the data
    if mat[0] == []:
        return(None)#if your input is empty,return empty
    else:
        maap = []
        for i in range(len(mat)):
            if mat[i] == []:
                return(None) #this is a part to judge if the lines are empty
            else:
                if mat[i][0] != '$':
                    maap.append(mat[i])#maap is the map without comment, we will use it some time after.
        chushi = []
        chushi.append(int(maap[0][0]))
        chushi.append(int(maap[0][1]))#chushi is the position of start cell,and it haven't been changed into (lines,rows)
        zhongzhi = []
        zhongzhi.append(int(maap[1][0]))
        zhongzhi.append(int(maap[1][1]))#chushi is the position of end cell,and it haven't been changed into (lines,rows)
        maap.remove(maap[0])
        maap.remove(maap[0])#this two steps will delete the start and end cell
        mapp = []
        for i in range(len(maap)): #len maap is the length of the array maap
            hang = []
            yihang = maap[i][0]
            for j in yihang :
                hang.append(j)
            mapp.append(hang) #we can use these loop to add the "."and"X"to the real map
        ma = [chushi,zhongzhi,mapp]
        return(ma) #the result of this function is the start,end,and the real map

def newmap(mapp): #this function defined a array used to save the distances
    new = []
    for i in range(len(mapp)):
        a = []
        for j in range(len(mapp[0])):
            a.append(float('inf'))#this loop can reach all the array,and make the value of each of them "infinite"
        new.append(a)
    return(new) #the result is the array with all "infinite"

def distance(new,start,mapp,end): #we use this function to find the distance between a random cell and the start cell
    if mapp != []:
        n = len(mapp[0])
        m = len(mapp)
        h = m - start[1] - 1
        l = start[0]
        if mapp[h][l] == 'X':
            return(new) #if the start or the end if not vaild, then we must return the initial map
        else:
            st = [[h,l]]
            new[h][l] = 0
            while len(st) > 0:
                hn = adddis(st,new,n,m,mapp) #else, we must use another function called"add distance"
                st = hn#then we get the distance of the neighbor cells
            return(new)
    else:
        return(None) #this is a special case, that means, the map is empty!

def path(start,end,dis,mapp): # the function path provide us a true way to the end cell and rewirte the map with"P"
    n = len(mapp[0])
    m = len(mapp)
    zdh = m - end[1] - 1
    zdl = end[0]
    qdh = m - start[1] - 1
    qdl = start[0] #we use these value to save the position which has been changed into lines and rows
    if dis[zdh][zdl] == float('inf'):
        return(mapp) #if we can find a way to reach the end, we must return the initial map
    else:
        while (qdh != zdh) or (qdl != zdl):
            i = zdh#the rows of map
            j = zdl#the line of map
            BB = minposition(i,j,dis,mapp,m,n)#this function used to find a minium distance of a point with their neighbors
            zdh = BB[0]
            zdl = BB[1]
            mapp[zdh][zdl] = 'P'#we rerwrite it with P
        return(mapp)
            
def minposition(i,j,dis,mapp,m,n): #in this function, we know a point and have to find the nearest neighborpoints
    A = float('inf')
    B = []
    mapp[i][j] = 'P' #distance is already known
    if (i+1)<m : 
        if dis[i+1][j]<A: #we ergodic all the neighbors clockwise and find the nearest one(under)
            A = dis[i+1][j]
            B = [i+1,j]
    if (i+1)<m and (j-1)>= 0: #we ergodic all the neighbors clockwise and find the nearest one(left,under)
        if dis[i+1][j-1]<A:
            A = dis[i+1][j-1]
            B = [i+1,j-1]
    if (j-1)>= 0: #we ergodic all the neighbors clockwise and find the nearest one(left)
        if dis[i][j-1]<A:
            A = dis[i][j-1]
            B = [i,j-1]
    if (i-1)>= 0 and (j-1)>= 0: #we ergodic all the neighbors clockwise and find the nearest one(left,up)
        if dis[i-1][j-1]<A:
            A = dis[i-1][j-1]
            B = [i-1,j-1]
    if (i-1)>= 0: #we ergodic all the neighbors clockwise and find the nearest one(up)
        if dis[i-1][j]<A:
            A = dis[i-1][j]
            B = [i-1,j]
    if (i-1)>= 0 and (j+1)<n: #we ergodic all the neighbors clockwise and find the nearest one(right,up)
        if dis[i-1][j+1]<A:
            A = dis[i-1][j+1]
            B = [i-1,j+1]
    if (j+1) < n: #we ergodic all the neighbors clockwise and find the nearest one(right)
        if dis[i][j+1]<A:
            A = dis[i][j+1]
            B = [i,j+1]
    if (i+1)<m and (j+1)<n: #we ergodic all the neighbors clockwise and find the nearest one(right,under)
        if  dis[i+1][j+1]<A:
            A = dis[i+1][j+1]
            B = [i+1,j+1]
    return(B)

def adddis(st,new,n,m,mapp): #in this function we find the distance from a cell to the neighbors
    hn = []
    for i in range(len(st)):
        hh = st[i][0]
        ll = st[i][1] #the code below used to use the dijsktra to find the true distance
        if (hh+1 < m)and (mapp[hh+1][ll] != 'X') and (new[hh][ll] + 1 < new[hh+1][ll]):
            new[hh+1][ll] = new[hh][ll] + 1
            hn.append([hh+1,ll]) #we ergodic all the neighbors clockwise and find the distance of the two points
        if (ll+1 < n)and (mapp[hh][ll+1] != 'X') and (new[hh][ll] + 1 < new[hh][ll+1]):
            new[hh][ll+1] = new[hh][ll] + 1
            hn.append([hh,ll+1])#we ergodic all the neighbors clockwise and find the distance of the two points
        if (ll-1 >= 0)and (mapp[hh][ll-1] != 'X') and (new[hh][ll] + 1 < new[hh][ll-1]):
            new[hh][ll-1] = new[hh][ll] + 1
            hn.append([hh,ll-1])#we ergodic all the neighbors clockwise and find the distance of the two points
        if (hh-1 >= 0)and (mapp[hh-1][ll] != 'X') and (new[hh][ll] + 1 < new[hh-1][ll]):
            new[hh-1][ll] = new[hh][ll] + 1
            hn.append([hh-1,ll])#we ergodic all the neighbors clockwise and find the distance of the two points
        if ((hh-1) >= 0)and ((ll-1) >= 0) and (mapp[hh-1][ll-1] != 'X') and ((new[hh][ll] + 1.414) < new[hh-1][ll-1]):
            new[hh-1][ll-1] = (new[hh][ll] +1.414)
            hn.append([hh-1,ll-1])#we ergodic all the neighbors clockwise and find the distance of the two points
        if ((hh-1) >= 0)and ((ll+1) < n) and (mapp[hh-1][ll+1] != 'X') and ((new[hh][ll] + 1.414) < new[hh-1][ll+1]):
            new[hh-1][ll+1] = (new[hh][ll] + 1.414) 
            hn.append([hh-1,ll+1])#we ergodic all the neighbors clockwise and find the distance of the two points
        if ((hh+1) < m)and ((ll-1) >= 0)and (mapp[hh+1][ll-1] != 'X') and ((new[hh][ll] + 1.414) < new[hh+1][ll-1]):
            new[hh+1][ll-1] = (new[hh][ll] +1.414)
            hn.append([hh+1,ll-1])#we ergodic all the neighbors clockwise and find the distance of the two points
        if ((hh+1) < m)and ((ll+1) < n) and (mapp[hh+1][ll+1] != 'X') and ((new[hh][ll] + 1.414) < new[hh+1][ll+1]):
            new[hh+1][ll+1] = (new[hh][ll] +1.414)
            hn.append([hh+1,ll+1])#we ergodic all the neighbors clockwise and find the distance of the two points
    for t in range(len(hn)):
        hn[t] = tuple(hn[t])
    hn = list(set(hn)) #we remove the same items of the set
    return(hn)

if __name__ == '__main__': #this is our main program
    maap = readlines()
    if maap == [[]]:
        print() #for the empty map, we return empty
    else:
        mappz = yaode(maap)
        if mappz == None:
            print() #for the empty map, we return empty
        else:
            start = mappz[0]
            end = mappz[1]
            mapp = mappz[2] #the map start position and end position
            if start == end: #this is a case that the start and the end is the same point
                m = len(mapp)
                qdh = m - start[1] - 1
                qdl = start[0] #shift the position into a right form
                mapp[qdh][qdl] = 'P' #rewrite the start cell with P
                pat = mapp
                for i in range(len(pat)):
                    strr = ''
                    for j in range(len(pat[0])):
                        strr = strr + str(pat[i][j]) #strr is a string saved the information of the map with path
                    print(strr)
            else:
                new = newmap(mapp)
                dis = distance(new,start,mapp,end) #we must compute the distance at first
                if mapp != []:
                    pat = path(start,end,dis,mapp) #then we find a possible path
                    for i in range(len(pat)):
                        strr = ''
                        for j in range(len(pat[0])):
                            strr = strr + str(pat[i][j])
                        print(strr)#strr is a string saved the information of the map with path
                else:
                    print('')
