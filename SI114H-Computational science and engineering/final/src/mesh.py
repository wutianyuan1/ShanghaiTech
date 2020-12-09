import numpy as np
from scipy.spatial import Delaunay

class Mesh(object):
    def __init__(self, dirichletBound, neumannBound):
        self._dirichletBound = dirichletBound.copy()
        self._neumannBound = neumannBound.copy()
        self._allBound = list(set(self._dirichletBound) | set(self._neumannBound))
        self._mesh = []
    
    def generate(self):
        pass

    def plot(self):
        pass


def test():
    db = [(i, 0) for i in np.arange(0, 1, 0.02)] + [(0, i) for i in np.arange(0, 1, 0.02)]
    nb = [(i, 1) for i in np.arange(0, 1, 0.02)] + [(1, i) for i in np.arange(0, 1, 0.02)]
    m = Mesh(db, nb)
    m.generate()
    m.plot()

if __name__ == "__main__":
    test()