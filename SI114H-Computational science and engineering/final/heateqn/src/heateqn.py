import numpy as np
import scipy
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.animation import FuncAnimation
from tqdm import tqdm
from math import sin, cos, pi, sqrt, ceil
from scipy.sparse import csr_matrix
from scipy.sparse.linalg import lsqr

# Triangluation provided by scipy
from scipy.spatial import Delaunay
# Our own triangulation implementation
# from delaunay import Delaunay

class Problem(object):
    def __init__(self, dirichletCoords, neumannCoords, initialState, neumannBoundFunc, dirichletBoundFunc, extForceFunc, maxdis):
        self.dirichletCoords    = np.array(dirichletCoords)
        self.neumannCoords      = np.array(neumannCoords)
        self.allCoords          = dirichletCoords + neumannCoords
        self.dirichletIdxs      = np.arange(0, len(dirichletCoords))
        self.neumannIdxs        = np.arange(len(dirichletCoords) - 1, len(self.allCoords))
        self.initialState       = initialState
        self.neumannBoundFunc   = neumannBoundFunc
        self.dirichletBoundFunc = dirichletBoundFunc
        self.extForceFunc       = extForceFunc
        self.triangleIdxs       = []
        self.numVertices        = 0
        self.maxdis             = maxdis
        self.dirichletEdges     = np.zeros( (len(self.dirichletIdxs), 2), dtype=np.int )
        self.neumannEdges       = np.zeros( (len(self.neumannIdxs), 2), dtype=np.int )
        for i in self.dirichletIdxs:
            self.dirichletEdges[i][0] = i
            self.dirichletEdges[i][1] = (i + 1) % len(self.allCoords)
        for i in self.neumannIdxs:
            self.neumannEdges[i - self.neumannIdxs[0]][0] = i
            self.neumannEdges[i - self.neumannIdxs[0]][1] = (i + 1) % len(self.allCoords)
        self.triangulate()

    def triangulate(self):
        ###################### Case 2 ######################
        self.allCoords.append([2, 2]) # Center, improve performance

        triangles = Delaunay(self.allCoords)
        recurse = False
        edgeSet = set()
        for triangle in triangles.simplices:
            if (triangle[1], triangle[0]) not in edgeSet:
                edgeSet.add((triangle[0], triangle[1]))
            if (triangle[2], triangle[1]) not in edgeSet:
                edgeSet.add((triangle[1], triangle[2]))
            if (triangle[0], triangle[2]) not in edgeSet:
                edgeSet.add((triangle[2], triangle[0]))
        
        for e in edgeSet:
            p1, p2 = self.allCoords[e[0]], self.allCoords[e[1]]
            d = sqrt((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2)
            if d > 2 * self.maxdis:
                recurse = True
                for i in range(1, ceil(d / self.maxdis)):
                    ratio = i / (d / self.maxdis)
                    self.allCoords.append( [p1[0] - ratio * (p1[0] - p2[0]), p1[1] - ratio * (p1[1] - p2[1])] )
        if recurse:
            self.triangulate()
        else:
            self.triangleIdxs = triangles.simplices
            self.numVertices = len(self.allCoords)
            # Convert to numpy array when done
            self.allCoords = np.array(self.allCoords) 
    
    def show(self, triangle=False):
        if len(self.dirichletEdges) > 0 and len(self.dirichletCoords) > 0:
            plt.scatter(self.dirichletCoords[:, 0], self.dirichletCoords[:, 1], c='b')
            for edge in self.dirichletEdges:                
                p1, p2 = self.allCoords[edge[0]], self.allCoords[edge[1]]
                plt.plot([p1[0], p2[0]], [p1[1], p2[1]], c='b')

        if len(self.neumannEdges) > 0 and len(self.neumannCoords) > 0:
            plt.scatter(self.neumannCoords[:, 0], self.neumannCoords[:, 1], c='r')
            for edge in self.neumannEdges:
                p1, p2 = self.allCoords[edge[0]], self.allCoords[edge[1]]
                plt.plot([p1[0], p2[0]], [p1[1], p2[1]], c='r', linestyle='--')
        if triangle:
            plt.triplot(self.allCoords[:, 0], self.allCoords[:, 1], self.triangleIdxs, c = 'g')
        plt.show()
    

class ProblemGenerator(object):
    @staticmethod
    def s_neumFunc(coords, t):
        return np.zeros(len(coords))

    @staticmethod
    def s_diricFunc(coords, t):
        return np.zeros(len(coords))

    @staticmethod
    def s_extForce(coords, t):
        ###################### Case 1 ######################
        # res = np.zeros(len(coords))
        ###################### Case 2 ######################
        res = 10 * np.ones(len(coords))
        return res

    @staticmethod
    def s_initialState(coords):
        res = np.zeros(len(coords))
        ###################### Case 2 ######################
        # for i, c in enumerate(coords):
        #     res[i] = cos(c[0]) * cos(c[1])
        return res

    @classmethod
    def gen(cls):
        ###################### Case 1 ######################
        # dirichletCoords = []
        # delta = pi / 10
        # y0 = pi / 2; x0 = - pi / 2 + delta; 
        # while y0 >= -pi / 2:
        #     dirichletCoords.append([-pi / 2, y0])
        #     y0 -= delta
        # while x0 <= pi / 2 - delta:
        #     dirichletCoords.append([x0, -pi / 2])
        #     x0 += delta
        # y0 = -pi / 2; x0 = pi / 2  - delta
        # while y0 <= pi / 2:
        #     dirichletCoords.append([pi / 2, y0])
        #     y0 += delta
        # while x0 >= -pi / 2 + delta:
        #     dirichletCoords.append([x0, pi / 2])
        #     x0 -= delta
        # neumannCoords = []

        ###################### Case 2 ######################
        dirichletCoords = []
        theta, dtheta = pi / 4, 0.05 * pi
        while abs(theta - (-5 * pi / 4 - dtheta)) > 0.05 * dtheta:
            x, y = 2 * cos(theta) + 2, 2 * sin(theta) + 2
            dirichletCoords.append([x, y])
            theta -= dtheta
        neumannCoords = []
        theta = 3 * pi / 4 - dtheta
        while abs(theta - (pi / 4)) > 0.05 * dtheta:
            x, y = 2 * cos(theta) + 2, 2 * sin(theta) + 2
            neumannCoords.append([x, y])
            theta -= dtheta

        maxdis = sqrt((dirichletCoords[0][0] - dirichletCoords[1][0])**2 + (dirichletCoords[0][1] - dirichletCoords[1][1])**2)
        return Problem(dirichletCoords, neumannCoords, cls.s_initialState, cls.s_neumFunc, cls.s_diricFunc, cls.s_extForce, maxdis)


class FEM_Solver(object):
    def __init__(self, dt, t_interval, problem):
        self.dt = dt
        self.t_interval = t_interval
        self.problem = problem
        self.M = self._construct_M()
        self.A = self._construct_A()
    
    def _triangleStiffMat(self, tri):
        x1, x2, x3, y1, y2, y3 = tri[0][0], tri[1][0], tri[2][0], tri[0][1], tri[1][1], tri[2][1]
        K = np.array( [[1, 1, 1], [x1, x2, x3], [y1, y2, y3]] )
        J = np.array( [[x1 -x3, x2 -x3], [y1 -y3, y2 - y3]] )
        G = np.dot( np.linalg.inv(K), np.array([[0, 0], [1, 0], [0, 1]]) )
        stiff = 0.5 * np.linalg.det(J) * np.dot(G, G.transpose())
        return stiff

    def _construct_M(self):
        M = np.zeros((self.problem.numVertices, self.problem.numVertices))
        for triIdx in self.problem.triangleIdxs:
            tri = self.problem.allCoords[triIdx, :]
            x1, x2, x3, y1, y2, y3 = tri[0][0], tri[1][0], tri[2][0], tri[0][1], tri[1][1], tri[2][1]
            K = np.array( [[1, 1, 1], [x1, x2, x3], [y1, y2, y3]] )
            M[triIdx[:, np.newaxis], triIdx] += (np.linalg.det(K) * np.array([[2, 1, 1], [1, 2, 1], [1, 1, 2]]) ) / 24.0
        return M

    def _construct_A(self):
        A = np.zeros((self.problem.numVertices, self.problem.numVertices))
        for triIdx in self.problem.triangleIdxs:
            A[triIdx[:, np.newaxis], triIdx] += self._triangleStiffMat(self.problem.allCoords[triIdx, :])
        return A
    
    def _step(self, tstep, prev):
        b = np.zeros((self.problem.numVertices))
        # External force 
        for triIdx in self.problem.triangleIdxs:
            tri = self.problem.allCoords[triIdx, :]
            x1, x2, x3, y1, y2, y3 = tri[0][0], tri[1][0], tri[2][0], tri[0][1], tri[1][1], tri[2][1]
            K = np.array( [[1, 1, 1], [x1, x2, x3], [y1, y2, y3]] )
            b[triIdx] += np.linalg.det(K) * self.dt * self.problem.extForceFunc(self.problem.allCoords[triIdx, :], tstep) / 6.0
        # Neumann boundary conditions
        for edge in self.problem.neumannEdges:
            b[edge] += np.linalg.norm(self.problem.allCoords[edge[0]] - self.problem.allCoords[edge[1]]) * self.dt \
                        * self.problem.neumannBoundFunc(self.problem.allCoords[edge], tstep) / 2.0
        # b = b + MT(n-1)
        b = b + np.dot(self.M, prev)
        # fix dirichlet conditions
        res = np.zeros(len(self.problem.allCoords))
        # Fix dirichlet conditions
        res[self.problem.dirichletIdxs] = self.problem.dirichletBoundFunc(self.problem.dirichletCoords, tstep)
        # Solve other nodes via lsqr
        freeNodes = [i for i in range(len(self.problem.dirichletIdxs), len(self.problem.allCoords)) ]
        Q = csr_matrix( self.A[np.ix_(freeNodes, freeNodes)] * self.dt + self.M[np.ix_(freeNodes, freeNodes)] )
        res[freeNodes] = lsqr(Q, b[freeNodes])[0]
        return res

    def _show_update(self, fig, ax, i):
        plt.clf()
        tpc = plt.tripcolor(self.problem.allCoords[:,0], self.problem.allCoords[:,1], self.problem.triangleIdxs, self.result[i, :], cmap=cm.jet, linewidth=0.2)
        fig.colorbar(tpc)
        if i == len(self.result) - 1:
            exit(0)

    def solve(self):
        nsteps = int(self.t_interval / self.dt)
        result = np.zeros((nsteps + 1, self.problem.numVertices))
        result[0, :] = self.problem.initialState(self.problem.allCoords)
        for i in tqdm(range(1, 1 + nsteps)):
            result[i, :] = self._step(i * self.dt, result[i-1, :])
        self.result = result
        return result
    
    def sp_error_analysis(self):
        solved = self.result.copy()
        actual = np.zeros(solved.shape)
        actual[0, :] = self.result[0, :]
        for i in range(1, len(actual)):
            for j in range(len(self.problem.allCoords)):
                x, y = self.problem.allCoords[j]
                actual[i, j] = np.cos(x) * np.cos(y) * np.exp(-2 * i * self.dt)
        err = np.zeros(len(actual))
        for i in range(len(err)):
            ac = actual[i]
            sl = solved[i]
            # 40 dirichlet fixed nodes in total, where must be zero
            # skip the grids where actual and solved is zero (boundary nodes)
            # otherwise, a 'ZeroDivisionError' will occur, or make this analysis meaningless
            err[i] = np.mean( np.abs(sl[40:] - ac[40:]) / ac[40:] ) 

        line, = plt.plot(np.arange(len(err)), err, c='black')
        scat = plt.scatter(np.arange(len(err)), err, c='b')
        line.set_zorder(0)
        plt.xlabel('time(t)')
        plt.ylabel('error')
        plt.title('Error analysis for a specific Heat Equation')
        plt.show()
        
    def show_last_frame(self):
        plt.clf()
        tpc = plt.tripcolor(self.problem.allCoords[:,0], self.problem.allCoords[:,1], self.problem.triangleIdxs, self.result[-1, :], cmap=cm.jet, linewidth=0.2)
        plt.colorbar(tpc)
        plt.xlabel('x')
        plt.ylabel('y')
        plt.title('Solution at t = {}, with dt = {}'.format(self.t_interval, self.dt))
        plt.show()

    def show_animate(self):
        try:    
            fig, ax = plt.subplots()  
            ani = FuncAnimation(fig, lambda i : self._show_update(fig, ax, i), 
                                range(len(self.result)), init_func=lambda: self._show_update(fig, ax, 0), interval=30)
            plt.show()
        except AttributeError:
            print("Result is empty, please run `solve` first")
            exit(1)

def main():
    p = ProblemGenerator.gen()
    sol = FEM_Solver(0.01, 1, p)
    sol.solve()
    sol.show_last_frame()
    
if __name__ == "__main__":
    main()
