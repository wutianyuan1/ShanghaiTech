import matplotlib.pyplot as plt
import numpy as np
import re, os
from sys import argv

s = """
                Core t (s)   Wall t (s)        (%)
       Time:     3390.812       56.521     5999.2
                 (ns/day)    (hour/ns)
Performance:       18.591        1.291
Finished mdrun on rank 0 Tue Nov 10 05:48:42 2020
"""

arg_combs = [(60, 1), (30, 2), (20, 3), (15, 4), (12, 5), (10, 6), (6, 10), (5, 12), (4, 15), (3, 20)]

def read_data(protein_name):
    y_avg, y_raw, y_var, x = [], [], [], []
    logfile = open(protein_name + '_scaling_profile_noGPU.dat', 'w')
    logfile.write('MPI-rank    OMP-thread-num   ns/day\n')
    for (ntmpi, ntomp) in arg_combs:
        dirname = "cpu-{}-{}-{}".format(protein_name, ntmpi, ntomp)
        x.append(ntmpi)
        y_tmp_raw = []
        for i in range(5):
            path_of_log = dirname + "/" + str(i) + ".md.log"
            if not os.path.exists(path_of_log):
                print("FILE NOT FOUND: ", path_of_log)
                continue
            print("parsing: ", path_of_log)
            with open(path_of_log) as f:
                content = f.read()
            params = (re.findall("Performance.*", content)[0]).split()
            # print(params[1])
            logfile.write('{}    {}    {}\n'.format(ntmpi, ntomp, params[1]))
            y_tmp_raw.append(float(params[1]))
        # try:
        y_raw.append(y_tmp_raw)
        y_avg.append(np.mean(y_tmp_raw))
        y_var.append(np.var(y_tmp_raw))
        print("Current data:", y_raw[-1], y_avg[-1], y_var[-1])
        # except:
        # print("Eroror during calculating")
    logfile.close()
    if len(y_raw) == 0:
        return x, [0]*5, [[0]*5]*5, [0]*5
    return x, y_raw, y_avg,np.sqrt(y_var)

def draw():
    title = argv[1] + "_scaling_profile_noGPU"
    x, _, y_avg, y_var = read_data(argv[1])
    print("draw: ", y_avg, y_var)
    plt.errorbar(x[::-1], y_avg[::-1], y_var[::-1], fmt='o', elinewidth=3, capsize=0, color="black", ecolor="lightgray")
    plt.plot(x[::-1], y_avg[::-1], color='lightgray')
    print("x: ", x[::-1])
    plt.xlabel("MPI rank")
    plt.ylabel("ns/day")
    plt.title(title)
    plt.savefig(title + '.png')

# print(read_data(argv[1]))
draw()  


