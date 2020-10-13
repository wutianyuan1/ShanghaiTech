import re
import matplotlib.pyplot as plt
import numpy as np
from random import uniform, random
from math import sin, cos, log, sqrt, pi, exp
import sys
sys.setrecursionlimit(1000000)

def normal_acc_rej(*args):
    def expo_1():
        u = uniform(0, 1)
        return -log(u)
    y = expo_1()
    u = uniform(0, 1)
    if u < exp(-0.5*(y-1)**2):
        return y if random() < 0.5 else -y
    return normal_acc_rej()

def normal(*args):
    u1 = uniform(0,1)
    u2 = uniform(0,1)
    return sqrt(-2*log(u1))*cos(2*pi*u2)

def logistic(*args):
    u = uniform(0, 1)
    return log(u/(1-u))

def rayleigh(*args):
    u = uniform(0, 1)
    return sqrt(-2*log(1-u))

def beta(a, b):
    def order_stat(_n, _j):
        l = sorted([uniform(0,1) for _ in range(_n)])
        return l[_j-1]

    def beta_int(_a, _b):
        return order_stat(_a+_b-1, _a)

    if a < 1 and b < 1:
        u1, u2 = uniform(0, 1), uniform(0, 1)
        x = u1**(1/a); y = u2**(1/b)
        if x + y < 1:
            return x/(x+y)
        return beta(a, b)
    else:
        return beta_int(a, b)    


def logistic_line(*args):
    x = np.arange(-10, 10, 0.01)
    y = np.array([exp(-xx)/(1+exp(-xx))**2 for xx in x])
    return (x, y)

def rayleigh_line(*args):
    x = np.arange(0, 6, 0.01)
    y = np.array([xx*exp(-(xx**2)/2) for xx in x])
    return (x, y)

def normal_line(*args):
    x = np.arange(-6, 6, 0.01)
    y = np.array([(1/sqrt(2*pi))*exp(-(xx**2)/2) for xx in x])
    return (x, y)

def beta_line(a, b):
    def beta_integral(a, b):
        I = 0
        for i in np.arange(0.001, 1, 0.001):
            I += (i**(a-1))*((1-i)**(b-1))*0.001
        return I
    coef = 1/beta_integral(a, b)
    print(coef)
    x = np.arange(0.001, 1, 0.001)
    y = np.array([float( coef*(xx**(a-1))*((1-xx)**(b-1)) ) for xx in x])
    return (x, y)

def plot(distribution_func, line_func, repeat, figname, *params, isCDF=False):
    ys = np.array([distribution_func(*params) for _ in range(repeat)])
    plt.hist(ys, bins=70, density=1, facecolor="blue", edgecolor="black", alpha=0.7, cumulative=isCDF)
    plt.title("PDF and histogram of %s distribution" % figname)
    plt.xlabel("x"); plt.ylabel("Normed frequency & Probability")
    x, y = line_func(*params)
    plt.plot(x, y, color='r', linewidth=2.5)
    plt.show()

def plot_beta():
    y1 = np.array([beta(0.5, 0.5) for _ in range(200000)])
    ax = plt.subplot(221)
    ax.hist(y1, bins=70, density=1, facecolor="blue", edgecolor="black", alpha=0.7, cumulative=False)
    x, y = beta_line(0.5, 0.5)
    ax.plot(x, y, c="r", linewidth=2.5)
    ax.set_title("PDF and histogram of %s distribution" % "Beta(0.5, 0.5)")
    ax.set_xlabel("x"); ax.set_ylabel("Normed frequency & Probability")

    y2 = np.array([beta(2, 8) for _ in range(200000)])
    ax = plt.subplot(222)
    ax.hist(y2, bins=70, density=1, facecolor="blue", edgecolor="black", alpha=0.7, cumulative=False)
    x, y = beta_line(2, 8)
    ax.plot(x, y, c="r", linewidth=2.5)
    ax.set_title("PDF and histogram of %s distribution" % "Beta(2, 8)")
    ax.set_xlabel("x"); ax.set_ylabel("Normed frequency & Probability")

    y3 = np.array([beta(1, 1) for _ in range(200000)])
    ax = plt.subplot(223)
    ax.hist(y3, bins=70, density=1, facecolor="blue", edgecolor="black", alpha=0.7, cumulative=False)
    x, y = beta_line(1, 1)
    ax.plot(x, y, c="r", linewidth=2.5)
    ax.set_title("PDF and histogram of %s distribution" % "Beta(1, 1)")
    ax.set_xlabel("x"); ax.set_ylabel("Normed frequency & Probability")

    y4 = np.array([beta(5, 5) for _ in range(200000)])
    ax = plt.subplot(224)
    ax.hist(y4, bins=70, density=1, facecolor="blue", edgecolor="black", alpha=0.7, cumulative=False)
    x, y = beta_line(5, 5)
    ax.plot(x, y, c="r", linewidth=2.5)
    ax.set_title("PDF and histogram of %s distribution" % "Beta(5, 5)")
    ax.set_xlabel("x"); ax.set_ylabel("Normed frequency & Probability")

    plt.show()

def birthday_exp():
    ''' Problem 7 '''
    E = 0
    for k in range(2, 365):
        prod = (k-1)/365
        for j in range(k-1):
            prod *= (365-j)/365
        E += k*prod
    return E

def std_sample():
    N, gt8 = 500000, 0
    samples = np.array([normal() for _ in range(N)])
    for i in range(N):
        if samples[i] > 8:
            gt8 += 1
    print("The tail probability evaluated by Standard Sample Average method is: %.6f" % (gt8/N))

def importance_sample():
    mu, N = 8, 500000
    samples = np.random.normal(mu, 1, N)
    P = sum(exp(0.5*mu**2 - mu*xk) for xk in samples if xk > mu) / N
    print("The tail probability evaluated by Importance sampling method is: {}".format(P))


def main():
    print(birthday_exp())
    

if __name__ == "__main__":
    main()
