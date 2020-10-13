import numpy as np 
import random 
from copy import deepcopy
from math import sqrt, log, exp 
from numba import jit
from collections import Counter
import matplotlib.pyplot as plt 

class Bandit:
    class Arm:
        def __init__(self, prob=0):
            self.p = prob

        def get(self):
            return 1 if np.random.uniform(0, 1) < self.p else 0
            
    def __init__(self, probs):
        self.arms = [Bandit.Arm(prob) for prob in probs]
    
    def get(self, arm_no):
        return self.arms[arm_no].get()


def eps_greedy(N, eps, bandit):
    theta = [0, 0, 0]
    count = [0, 0, 0]
    choices = np.array([0]*N)
    rewards = np.array([0]*N)
    total_reward = 0
    for i in range(N):
        It = 0
        if random.uniform(0, 1) < eps:
            It = random.choice([0, 1, 2])
        else:
            It = theta.index(max(theta))
        count[It] += 1
        choices[i] = It
        r_It = bandit.get(It)
        rewards[i] = r_It
        total_reward += r_It
        theta[It] += (1/count[It])*(r_It - theta[It])
    return theta, total_reward, choices, rewards

def UCB(N, c, bandit):
    count = [0, 0, 0]
    theta = [0, 0, 0]
    choices = np.array([0]*N)
    rewards = np.array([0]*N)
    total_reward = 0
    for t in range(3):
        count[t] = 1
        theta[t] = bandit.get(t)
        choices[t] = t
    for t in range(3, N):
        temp = [ (theta[j] + c*sqrt(2*log(t+1)/count[j]) ) for j in range(3) ]
        It = temp.index(max(temp))
        count[It] += 1
        choices[t] = It
        r_It = bandit.get(It)
        rewards[t] = r_It
        total_reward += r_It
        theta[It] += (1/count[It])*(r_It - theta[It])
    return theta, total_reward, choices, rewards

def TS(N, params, bandit):
    ret_params = deepcopy(params)
    total_reward = 0
    choices = np.array([0]*N)
    rewards = np.array([0]*N)
    for t in range(N):
        theta = [0, 0, 0]
        for j in range(3):
            theta[j] = np.random.beta(ret_params[j][0], ret_params[j][1])
        It = theta.index(max(theta))
        choices[t] = It
        r_It = bandit.get(It)
        rewards[t] = r_It
        total_reward += r_It
        ret_params[It][0] += r_It
        ret_params[It][1] += 1-r_It
    theta = [p[0]/(p[0] + p[1]) for p in ret_params]
    return theta, total_reward, choices, rewards

@jit
def choice(weights):
    It = -1
    rand = np.random.uniform(0, 1)
    if rand < weights[0]: 
        It = 0
    elif rand >= weights[0] and rand < weights[0] + weights[1]: 
        It = 1
    else: 
        It = 2
    return It

@jit
def softmax(l):
    mx = np.max(l)
    return np.exp(l-mx)/sum(np.exp(l-mx))

@jit(nopython=False)
def gradient(N, params, bandit):
    baseline, step = params[0], params[1]
    H = np.zeros((3), dtype=np.float)
    count = np.zeros((3), dtype=np.int)
    choices = np.array([0]*N)
    rewards = np.array([0]*N)
    avg_reward = 0
    for t in range(N):
        policy = softmax(H)
        It = choice(policy)
        choices[t] = It
        r_It = bandit.get(It)
        rewards[t] = r_It 
        count[It] += 1
        avg_reward = (t/(t+1))*avg_reward + (1/(t+1))*r_It 
        base = avg_reward if baseline == -1 else baseline
        for i in range(3):
            if i == It: 
                H[i] += step*(r_It - base)*(1 - policy[i])
            else: 
                H[i] -= step*(r_It - base)*policy[i]
    theta = [c/N for c in count]
    return theta, avg_reward*N, choices, rewards
    

def simulate(algorithm, N, repeat, params, figname, param_name):
    b = Bandit([0.9, 0.8, 0.7])
    colors = ['r', 'g', 'b', 'violet']
    lines, opts = [], []
    fig0 = plt.figure()
    fig1 = plt.figure()
    ax = [fig0.add_axes([0.1,0.1,0.8,0.8]), fig1.add_axes([0.1,0.1,0.8,0.8])]
    for rep in range(len(params)):
        th_total = [0, 0, 0]
        res  = np.zeros(shape=(repeat, N), dtype=np.int)
        res2 = np.zeros(shape=(repeat, N), dtype=np.int)
        reward_total = 0
        for ii in range(repeat):
            th, re, ch, rwd = algorithm(N, params[rep], b)
            for i in range(3):
                th_total[i] += th[i]
            for j in range(N):
                res[ii][j] = ch[j]
                res2[ii][j] = rwd[j]
            reward_total += re
            
        t = np.zeros(N)
        regret = np.zeros(N)
        opt_choices = np.zeros(N)
        overall_regret = 0
        for i in range(N):
            ith_choices = res[:, i]
            ith_rewards = res2[:, i]
            d = {0:[0,0], 1:[0,0], 2:[0,0]}
            for x in range(repeat):
                xch = ith_choices[x]
                d[xch][0] += 1 # number of choices
                d[xch][1] += ith_rewards[x] # rewards of chosen this
            t[i] = i
            for kk in range(3):      
                if d[kk][0] != 0: 
                    overall_regret += (d[kk][0]*(0.9-(d[kk][1]/d[kk][0])))/repeat          
            opt_choices[i] = d[0][0]/repeat
            regret[i] = overall_regret
        fig, = ax[0].plot(t, regret, c=colors[rep])
        opt, = ax[1].plot(t, opt_choices, c=colors[rep])
        lines.append(fig)
        opts.append(opt)
        print("Overall regret = {}".format(overall_regret))
        print("Average aggragate reward: {}".format(reward_total/repeat))
        print("Average theta: th_1 = {}, th_2 = {}, th_3 = {}".format(th_total[0]/repeat, th_total[1]/repeat, th_total[2]/repeat))
    ax[0].set_title(figname + " regret(t)")
    ax[1].set_title(figname + " optimal choices (%)")
    ax[0].legend(lines, [param_name + "={}".format(params[rep][0])  for rep in range(len(params))])
    ax[1].legend(opts , [param_name + "={}".format(params[rep][0])  for rep in range(len(params))])
    ax[0].set_ylabel("regret(t)")
    ax[0].set_xlabel("time step(t)")
    ax[1].set_ylabel("optimal choices (%)")
    ax[1].set_xlabel("time step(t)")
    plt.show()

  
def main():
    pass

if __name__ == "__main__":
    main()
