import numpy as np
import random
import copy
from collections import Counter
from math import pi, log, exp, sqrt

class MCTSNode(object):
    def __init__(self, parent, seq=None):
        self._parent = parent
        self._total_value = 0
        self._n_visits = 0
        self._seq = seq
        self.successors = {}
    
    def expand(self):
        # print("expand", self._seq)
        seq = self._seq
        for i, char in enumerate(seq):
            if (char == '0') and (i not in self.successors):
                newseq = seq[:i] + '1' + seq[i+1:]               
                self.successors[i] = MCTSNode(self, newseq)

    def get_next_node(self, c=5):
        maxnode, maxval = None, 0
        for succ in self.successors.values():
            UCB_res = float('inf')
            if succ._n_visits != 0:
                V_avg = (succ._total_value / succ._n_visits) + c*sqrt(log(self._n_visits)/ succ._n_visits)
                UCB_res = V_avg 
            if UCB_res > maxval:
                maxnode, maxval = succ, UCB_res
        return (maxnode, maxval)

    def update(self, val):
        self._n_visits += 1
        self._total_value += val
        if self._parent != None:
            self._parent.update(val)

    def best_successor(self, eval_fn):
        maxval, bestseq, node = 0, None, self
        while node.successors != {}:
            if eval_fn(node._seq) > maxval:
                bestseq = node._seq
                maxval = eval_fn(node._seq)
            node = node.get_next_node()[0]
        return maxval, bestseq
    
    def rollout_val(self, eval_fn, depth=1):
        newseq = copy.deepcopy(self._seq)
        for _ in range(depth-1):
            avail_idx = [i for i in range(len(newseq)) if newseq[i] == '0']
            rand_idx = random.choice(avail_idx)
            newseq = newseq[:rand_idx] + "1" + newseq[rand_idx+1:]
        self._n_visits += 1
        return eval_fn(newseq)

    def __str__(self):
        return "NODE: " + str(self._seq) + ", nvisit = " + str(self._n_visits) + ", val = " + str(self._total_value)

    def __repr__(self):
        return self.__str__()
        
class MaxCut(object):
    def reward(self, seq):
        adjlist = [[1, 4, 15], [0, 2, 14], [1, 3, 12], [2, 4, 7], [0, 3, 5], [4, 6, 17], [5, 7, 8],
        [6, 10, 3], [6, 9, 18], [8, 10, 11], [7, 9, 12], [9, 13, 19], [2, 10, 13], [12, 14, 11], 
        [1, 13, 16], [0, 16, 17], [14, 15, 19], [5, 15, 18], [8, 17, 19], [11, 16, 18]]
        res = 0
        assert(len(seq) == len(adjlist)) 
        for i in range(len(seq)):
            adj = adjlist[i]
            for j in range(len(adj)):
                if seq[i] != seq[ int(adj[j]) ]:
                    res += 1
        return res/2

    def print_res(self, seq, max_size):
        s1, s2 = "", ""
        for i in range(len(seq)):
            if seq[i] == '1': s1 += (str(i+1) + ",")
            else: s2 += (str(i+1) + ",")
        print("The maximum cut is:\n S1 = {%s},\n S2 = {%s},\n with reward = %d" %(s1[:-1], s2[:-1], max_size))

    def simulate_mcmc(self, step, beta=3):
        curr_seq = np.zeros(20, dtype=np.int8)
        d = {}
        for _ in range(step):
            str_seq = "".join(str(int(i)) for i in curr_seq)
            if str_seq not in d:
                d[str_seq] = 1
            else:
                d[str_seq] += 1
            idx = random.randint(0, 19)
            new_seq = curr_seq.copy()
            if new_seq[idx] == 0:
                new_seq[idx] = 1
            else:
                new_seq[idx] = 0
            filpProb = min(1, exp( beta*(self.reward(new_seq) - self.reward(curr_seq)) ) )
            if random.random() > filpProb:
                continue
            curr_seq = new_seq
        max_size, max_seq= 0, ""
        for key in d:
            n = self.reward(key)
            if n > max_size:
                max_size = n
                max_seq = key
        self.print_res(max_seq, max_size)
        return max_seq

    def get_seq_by_prob(self, problist):
        return "".join(["1" if random.random() < problist[i] else "0" for i in range(len(problist))])
    
    def simulate_cem(self, step, rep=50):
        problist = [0.05]*20
        seqs, pairs = None, None
        for i in range(step):
            seqs = [self.get_seq_by_prob(problist) for _ in range(rep)]
            pairs = sorted([(self.reward(seqs[i]), seqs[i]) for i in range(len(seqs))], reverse=True)
            # Top 50% results
            templist = [0]*20
            for i in range(rep//2):
                curr_seq = pairs[i][1]
                for j in range(len(curr_seq)):
                    templist[j] += int(curr_seq[j])
            for i in range(len(templist)):
                templist[i] /= (rep//2)
            problist = templist
        self.print_res(pairs[0][1], pairs[0][0])
        return pairs[0]

    def simulate_mcts(self, step, c=5):
        root = MCTSNode(None, "0"*20)
        root.expand()
        for _ in range(step):
            node = root
            while len(node.successors.values()) != 0:
                node, _ = node.get_next_node()
            if node._n_visits == 0:
                rollout_val = node.rollout_val(self.reward)
                node.update(rollout_val)
            else:
                node.expand()
        maxval, bestseq = root.best_successor(self.reward)
        self.print_res(bestseq, maxval)
            
            
m = MaxCut()

print("="*14 + "MCMC method" + "="*15)
s = m.simulate_mcmc(10000)

print("="*10 + "Cross Entropy method" + "="*10)
s = m.simulate_cem(100, 500)

print("="*14 + "MCTS method" + "="*15)
s = m.simulate_mcts(5000)
