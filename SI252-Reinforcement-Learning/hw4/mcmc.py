import numpy as np
import random
from collections import Counter
import matplotlib.pyplot as plt
from math import pow, exp
from mpl_toolkits.mplot3d import Axes3D

"""PleaseUseCamelCaseAndOOP"""

class BinSeqWithNoAdj1(object):
    def __init__(self, length):
        self.length = length
    
    def _checkGoodSeq(self, seq):
        for i in range(len(seq)-1):
            if seq[i] == '1' and seq[i+1] == '1':
                return False
        return True

    def simulate(self, step):
        currSeq = '0'*self.length
        totalNumOf1 = 0
        for _ in range(step):
            totalNumOf1 += Counter(currSeq)['1']
            idx = random.randint(0, self.length-1)
            newSeq = currSeq[:idx] + ('0' if currSeq[idx] == '1' else '1') + currSeq[idx+1:]  
            if self._checkGoodSeq(newSeq):
                currSeq = newSeq
        return totalNumOf1/step

class PowerLawDist(object):
    def simulate(self, step):
        state = 1
        self.res = np.zeros(step)
        self.step = step
        for i in range(step):
            self.res[i] = min(state, 9)
            if state == 1:
                if random.random() < pow(2, -2.5):
                    state = 2
                continue
            proposal = state + 1 if (random.random() < 0.5) else state - 1
            if proposal == state + 1:
                if random.random() < pow(state/(state+1), 1.5):
                    state = proposal
            else:
                state = proposal
        
    def show(self):
        cnter = Counter(self.res)
        for key in sorted(cnter.keys()):
            if key != 9:
                print("P(X = %d) = %f" % (key, cnter[key]/self.step))
            else:
                print("P(X >= %d) = %f" % (key, cnter[key]/self.step))
        plt.hist(self.res, bins=len(cnter.keys())-1, density=1)
        plt.title("Power Law Distribution")
        plt.xlabel("x"); plt.ylabel("p")
        plt.show()

class Knapsack(object):
    def __init__(self, maxWeight, weights, worths):
        self.weights = weights
        self.worths  = worths
        self.maxWeight = maxWeight
        self.length  = len(self.weights)
    
    def simulate(self, step, beta):
        currSeq = np.zeros(self.length)
        currWorth  = sum((currSeq[i]*self.worths[i])  for i in range(self.length))
        d = {}
        for _ in range(step):
            strSeq = "".join(str(int(i)) for i in currSeq)
            if strSeq not in d:
                d[strSeq] = [0, currWorth]
            else:
                d[strSeq][0] += 1
            idx = random.randint(0, self.length-1)
            newSeq = currSeq.copy()
            if newSeq[idx] == 0:
                newSeq[idx] = 1
            else:
                newSeq[idx] = 0
            newWeight = sum((newSeq[i]*self.weights[i]) for i in range(self.length))
            newWorth  = sum((newSeq[i]*self.worths[i])  for i in range(self.length))
            filpProb = min(1, exp(beta*(newWorth - currWorth)))
            if newWeight > self.maxWeight or random.random() > filpProb:
                continue
            currSeq = newSeq
            currWorth = newWorth
        bestSeq, maxWorth = sorted(list(d.items()), key=lambda x: x[1], reverse=True)[0]
        return bestSeq, maxWorth[1]

class Beta(object):
    def __init__(self, a, b):
        self.a = a
        self.b = b
    
    def simulate(self, step):
        w = np.random.uniform(0, 1)
        res = np.zeros(step)
        for i in range(step):
            res[i] = w
            u = np.random.uniform(0, 1)
            coin = (pow(u, self.a-1)*pow(1-u, self.b-1))/(pow(w, self.a-1)*pow(1-w, self.b-1))
            if np.random.uniform(0, 1) < min(1, coin):
                w = u
        plt.hist(res, bins=60, density=1, color='b', alpha=0.5)
        plt.title("Beta distribution (a = %d, b = %d)" %(self.a, self.b))
        plt.xlabel("x"); plt.ylabel("p")
        plt.show()

class StdNormal(object):
    def simulate(self, step):
        w = np.random.uniform(-10, 10)
        res = np.zeros(step)
        for i in range(step):
            res[i] = w
            u = np.random.uniform(-10, 10)
            coin = (exp(-0.5*u*u))/(exp(-0.5*w*w))
            if np.random.uniform(0, 1) < min(1, coin):
                w = u
        plt.hist(res, bins=100, density=1, color='b', alpha=0.5)
        plt.title("Standard Normal Distribution")
        plt.xlabel("x"); plt.ylabel("p")
        plt.show()

class NormNormConjugacy(object):
    def __init__(self, y, mu, sigma, tau):
        self.y = y; self.sigma = sigma
        self.mu = mu; self.tau = tau

    def simulate(self, step, d):
        w = np.random.normal(0, 1)
        res = np.zeros(step)
        for i in range(step):
            res[i] = w
            u = w + np.random.normal(0, d**2)
            num = (exp(-(1/(2*pow(self.sigma, 2)))*((self.y - u)**2)) ) *  (exp(-(1/(2*pow(self.tau, 2)))*((u - self.mu)**2)) )
            dom = (exp(-(1/(2*pow(self.sigma, 2)))*((self.y - w)**2)) ) *  (exp(-(1/(2*pow(self.tau, 2)))*((w - self.mu)**2)) )
            coin = num/dom
            if np.random.uniform(0, 1) < min(1, coin):
                w = u
        plt.hist(res, bins=100, density=1, color='b', alpha=0.5)
        plt.title("Simulation result with d = %f" % d)
        plt.xlabel("x"); plt.ylabel("p")
        plt.show()


class BiVarNormDist(object):
    def __init__(self, rho):
        self.rho = rho
    
    def simulate(self, step):
        x, y = 0, 0
        for _ in range(step):
            plt.scatter(x, y, c='blue')
            x = np.random.normal(self.rho*y, 1-self.rho**2)
            y = np.random.normal(self.rho*x, 1-self.rho**2)
        plt.xlabel("x"); plt.ylabel("y")
        plt.show()


class ChickenEgg(object):
    def __init__(self, a, b, lamb, x):
        self.a = a; self.b = b
        self.lamb = lamb; self.x = x

    def show(self, res):
        plt.hist(res, bins=60, density=1, color='b', alpha=0.5)
        plt.title("Chicken-Egg Gibbs")
        plt.xlabel("p"); plt.ylabel("Normalized frequency")
        plt.show()
        mean = np.mean(res)
        var  = np.var(res)
        print("Mean: %.4f, Variance: %.4f" %(mean, var))
        return mean, var

    def simulateMH(self, step):
        w = np.random.beta(self.a, self.b)
        res = np.zeros(step)
        for i in range(step):
            res[i] = w
            u = np.random.beta(self.a, self.b)
            num = exp(-self.lamb*u)*pow(self.lamb*u, self.x)*pow(u, self.a-1)*pow(1-u, self.b-1)
            dom = exp(-self.lamb*w)*pow(self.lamb*w, self.x)*pow(w, self.a-1)*pow(1-w, self.b-1)
            coin = num/dom
            if np.random.uniform(0, 1) < min(1, coin):
                w = u
        return self.show(res)

    def simulateGibbs(self, step):
        n = 100
        res = np.zeros(step)
        for i in range(step):
            p = np.random.beta(self.x+self.a, n-self.x+self.b)
            res[i] = p
            y = np.random.poisson(self.lamb*(1-p))
            n = self.x + y
        return self.show(res)


class ThreeDimJointDist(object):
    def simulate(self, step):
        x, p, n = 1, 0.5, 2
        res = np.zeros((step, 3))
        for i in range(step):
            res[i] = np.array([x, p, n])
            x = np.random.binomial(n, p)
            p = np.random.beta(x+1, n-x+1)
            n = np.random.poisson(4*(1-p)) + x
            
        ax = plt.figure().add_subplot(111, projection = '3d')
        ax.scatter(res[:, 0], res[:, 1], res[:, 2])
        ax.set_xlabel("X"); ax.set_ylabel("p"); ax.set_zlabel("N")
        plt.show()

class IndependentSet(object):
    def check(self, vertex):
        for i in range(len(vertex)):
            if i < 12:
                if vertex[i]&vertex[(i+4)%12] or vertex[i]&vertex[(i+8)%12] or vertex[i]&vertex[i+12]:
                    return False
            elif i != 23:
                if vertex[i]&vertex[i+1] or vertex[i]&vertex[i-1] or vertex[i]&vertex[i-12]:
                    return False
            else:
                if vertex[i]&vertex[12] or vertex[i]&vertex[i-1] or vertex[i]&vertex[i-12]:
                    return False
        return True

    def simulateContinuous(self, beta, step):
        currSeq = np.zeros(24, dtype=np.int8)
        d = set()
        for _ in range(step):
            strSeq = "".join(str(int(i)) for i in currSeq)
            if strSeq not in d:
                d.add(strSeq)
            idx = random.randint(0, 23)
            newSeq = currSeq.copy()
            if newSeq[idx] == 0:
                newSeq[idx] = 1
            else:
                newSeq[idx] = 0
            if not self.check(newSeq):
                continue
            pois = np.random.poisson(np.sum(newSeq))
            for _ in range(pois): # Stay at the same state
                pass
            currSeq = newSeq
        maxSize, maxSeq= 0, ""
        for key in d:
            n = Counter(key)['1']
            if n > maxSize:
                maxSize = n
                maxSeq = key
        print("The choice of maximum independent set is: %s, with size = %d" %(maxSeq, maxSize))
        return maxSeq


    def simulate(self, beta, step):
        currSeq = np.zeros(24, dtype=np.int8)
        d = set()
        for _ in range(step):
            strSeq = "".join(str(int(i)) for i in currSeq)
            if strSeq not in d:
                d.add(strSeq)
            idx = random.randint(0, 23)
            newSeq = currSeq.copy()
            if newSeq[idx] == 0:
                newSeq[idx] = 1
            else:
                newSeq[idx] = 0
            filpProb = min(1, exp(beta*(np.sum(newSeq) - np.sum(currSeq))))
            if (not self.check(newSeq)) or random.random() > filpProb:
                continue
            currSeq = newSeq
        maxSize, maxSeq= 0, ""
        for key in d:
            n = Counter(key)['1']
            if n > maxSize:
                maxSize = n
                maxSeq = key
        print("The choice of maximum independent set is: %s, with size = %d" %(maxSeq, maxSize))
        return maxSeq



    


def main():
    obj = IndependentSet()
    obj.simulate(3, 500000)
    

if __name__ == "__main__":
    main()

    