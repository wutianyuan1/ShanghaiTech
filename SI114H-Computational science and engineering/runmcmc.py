import numpy as np
import matplotlib.pyplot as plt
import random

x0 = np.matrix([-0.8, -0.8]).T
Nsamp = 10**4
d = 1/2

dim_x = max(x0.shape)
beta = 0.1
chain = np.zeros((dim_x, Nsamp))
chain[:,0] = x0.T

x = x0[::]
ar = 0

def postdis(x,d):
    sigma = 0.1
    G = forwardfun(x) 
    x_min = -1*np.ones(x.shape)
    x_max = 1*np.ones(x.shape)
    if (x>x_min).all() and (x<x_max).all():
        p = np.exp(-0.5*sum((G - d)**2)/sigma**2) 
    else:
        p = 0
    return p

def forwardfun(x):    
    return (x.T * x)

for n in range(Nsamp-1):
    z = x + beta*np.random.randn(dim_x, 1)
    px = postdis(x, d)
    pz = postdis(z, d)
    a = min(1, pz/px)
    if np.random.uniform() < a:
        x = z
        ar += 1
    chain[:, n+1] = x.T

ar = ar/Nsamp
plt.scatter(chain[0,:], chain[1,:])
plt.show()    

    

