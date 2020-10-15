import matplotlib.pyplot as plt
from math import pi

def derv(x):
    return [x[i] - x[i-1] for i in range(1, len(x))]

with open("angle_log.txt") as f:
    lines = [item for item in f.read().split('\n')[:-1]]
    lines = [l.split(',') for l in lines[100:]]
    alpha = [float(l[0])*180/pi - 180 for l in lines]
    beta  = [float(l[1])*180/pi for l in lines]
    gamma = [float(l[2])*180/pi for l in lines]
    x = [i for i in range(len(alpha))]
    plt.plot(x, alpha)
    plt.plot(x, beta)
    plt.plot(x, gamma)
    plt.legend(["alpha", "beta", "gamma"], loc='upper left')
    plt.title("Angles - Time after interploation")
    plt.xlabel("Time - t")
    plt.ylabel("Angle - degree")
    plt.show()


