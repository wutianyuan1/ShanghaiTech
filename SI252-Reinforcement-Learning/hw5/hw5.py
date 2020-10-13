import numpy as  np
import matplotlib.pyplot as plt
import random
from matplotlib.table import Table

def plot(image, title):
    _, ax = plt.subplots()
    ax.set_axis_off()
    tb = Table(ax, bbox=[0, 0, 1, 1])
    nrows, ncols = image.shape
    for (i,j), val in np.ndenumerate(image):
        tb.add_cell(i, j, 1/ncols, 1/nrows, text=val, loc='center', facecolor='white')
    ax.add_table(tb)
    ax.set_title(title)
    plt.show()

class GridWorld(object):
    def __init__(self, gamma):
        self.gamma = gamma
        self.eps = 0.0001
        self.worldSize = 5
        self.Apos = (0, 1)
        self.Asucc = (4, 1)
        self.Bpos = (0, 3)
        self.Bsucc = (2, 3)
        self.Areward = 10
        self.Breward = 5
        self.actions = [(-1, 0), (1, 0), (0, -1), (0, 1)]

    def get_successor(self, state, action):
        if state == self.Apos:
            return self.Asucc, self.Areward
        elif state == self.Bpos:
            return self.Bsucc, self.Breward
        else:
            newx, newy = state[0] + action[0], state[1] + action[1]
            if newx >= self.worldSize or newy >= self.worldSize or newx < 0 or newy < 0:
                return state, -1
            else:
                return (newx, newy), 0
    
    def BellmanExpectationEqn(self):
        world = np.zeros((self.worldSize, self.worldSize))
        while True:
            newWorld = np.zeros((self.worldSize, self.worldSize))
            for i in range(self.worldSize):
                for j in range(self.worldSize):
                    for act in self.actions:
                        newState, reward = self.get_successor((i, j), act)
                        newWorld[i, j] += 0.25*(reward + self.gamma * world[newState])
            if np.sum(np.abs(newWorld - world)) < self.eps:
                break
            world = newWorld
        return np.round(newWorld, decimals=1)

    def BellmanOptimalEqn(self):
        world = np.zeros((self.worldSize, self.worldSize))
        stateActionVals = np.zeros((self.worldSize, self.worldSize, 4))
        while True:
            newWorld = np.zeros((self.worldSize, self.worldSize))
            for i in range(self.worldSize):
                for j in range(self.worldSize):
                    for k, act in enumerate(self.actions):
                        newState, reward = self.get_successor((i, j), act)
                        stateActionVals[i, j, k] = reward + self.gamma*world[newState]
                    newWorld[i, j] = np.max(stateActionVals[i, j, :])
            if np.sum(np.abs(newWorld - world)) < self.eps:
                break
            world = newWorld
        return np.round(newWorld, decimals=1), np.round(stateActionVals, decimals=1)

gw = GridWorld(0.9)
stateVal = gw.BellmanExpectationEqn()
optVal, sav = gw.BellmanOptimalEqn()

# plot(stateVal, "State values under uniform random policy")
plot(optVal, "Optimal state values")


