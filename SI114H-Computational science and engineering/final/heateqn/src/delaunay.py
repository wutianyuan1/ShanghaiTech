import numpy as np

class Delaunay(object):
    def __init__(self, points):
        self.allCoords = [np.array((-100, -100)), np.array((100, -100)), np.array((-100, 100)), np.array((100, 100))]
        self.adjList = {}
        self.circles = {}
        Td, Tu = (0, 1, 2), (3, 2, 1)
        self.adjList[Td] = [Tu, None, None]
        self.adjList[Tu] = [Td, None, None]
        self.circles[Td] = self._circleParam(Td)
        self.circles[Tu] = self._circleParam(Tu)
        for i, p in enumerate(points):
            self.allCoords.append(p)
            self._update(p, i + 4)

    def _circleParam(self, tri):
        # Calculate the radius and center of the circle
        x1, x2, x3, y1, y2, y3 = self.allCoords[tri[0]][0], self.allCoords[tri[1]][0], self.allCoords[tri[2]][0],\
                                 self.allCoords[tri[0]][1], self.allCoords[tri[1]][1], self.allCoords[tri[2]][1]
        M = x1**2 - x2**2 + y1**2 - y2**2
        N = x1**2 - x3**2 + y1**2 - y3**2
        xab, xac, yab, yac = x1 - x2, x1 - x3, y1 - y2, y1 - y3
        xc = (M * yac - N * yab) / (2 * yac * xab - 2 * yab * xac)
        yc = (M * xac - N * xab) / (2 * yab * xac - 2 * yac * xab)
        r  = np.sqrt((x1 - xc)**2 + (y1 - yc)**2)
        return (xc, yc), r

    def _circleContains(self, t, p):
        c, r = self.circles[t]
        return np.sqrt((p[0] - c[0])**2 + (p[1] - c[1])**2) <= r
    
    def _update(self, p, pIdx):
        # Triangles which contains this point
        badTriangles = [tri for tri in self.adjList if self._circleContains(tri, p) == True]
        newTriangles = []
        s, edge, bds = badTriangles[0], 0, []
        # Find boundaries
        while True:
            adj = self.adjList[s][edge]
            if adj in badTriangles:
                adjOfadj = self.adjList[adj]
                edge = (adjOfadj.index(s) + 1) % 3
                s = adj
            else:
                bds.append( (s[(edge + 1) % 3], s[(edge - 1) % 3], adj) )
                edge = (edge + 1) % 3
                if bds[0][0] == bds[-1][1]:
                    break
        # Remove them (division needed)
        for bt in badTriangles:
            self.adjList.pop(bt)
            self.circles.pop(bt)
        # Create new triangles
        for bd in bds:
            edge0, edge1, triangle = bd[0], bd[1], bd[2]
            new = (pIdx, edge0, edge1)
            self.circles[new] = self._circleParam(new)
            self.adjList[new] = [triangle, None, None]
            if triangle != None:
                for i in range(len(self.adjList[triangle])):
                    currAdj = self.adjList[triangle][i]
                    if currAdj != None and edge0 in currAdj and edge1 in currAdj:
                        self.adjList[triangle][i] = new
            newTriangles.append(new)
        # Update adjacent list
        for i, nt in enumerate(newTriangles):
            self.adjList[nt][1] = newTriangles[(i + 1) % len(newTriangles)]
            self.adjList[nt][2] = newTriangles[(i - 1) % len(newTriangles)]
    
    @property
    def simplices(self):
        return np.array([(i1 - 4, i2 - 4, i3 - 4) for (i1, i2, i3) in self.adjList if i1 >= 4 and i2 >= 4 and i3 >= 4])

