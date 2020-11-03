import numpy as np
import matplotlib.pyplot as plt
from math import pi

def plot_step():
    x = np.arange(0, pi, 0.01)
    y = np.zeros(len(x))

    for i in range(501):
        b_j = (2/pi)*(2/(i+1))
        if i % 2 == 0:
            y += b_j*np.sin((i+1)*x)
        if i == 1 or i == 10 or i == 100 or i == 500:
            plt.plot(x, y)
    plt.legend(["k={}".format(str(item)) for item in [1, 10, 100, 500]] )
    plt.xlabel("x"); plt.ylabel("y")
    plt.title("Gibbs")
    plt.show()


def plot_delta():
    x = np.arange(-pi, pi, 0.01)
    y = np.zeros(len(x))
    fig_id = [1, 10, 100, 500]
    for i in range(501):
        y += (1/pi)*np.cos((i+1)*x)
        if i in fig_id:
            plt.subplot(2, 2, fig_id.index(i)+1)
            plt.plot(x, y)
            plt.legend(["k={}".format(str(fig_id[fig_id.index(i)]))] )
    plt.xlabel("x"); plt.ylabel("y")
    plt.title("Gibbs")
    plt.show()

plot_delta()
