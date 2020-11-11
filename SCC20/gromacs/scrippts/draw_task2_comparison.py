import numpy as np
import matplotlib.pyplot as plt
import os, sys

def read_xvg(filename, start_ln):
    ret_x, ret_y = [], []
    with open(filename) as f:
        content = f.readlines()
        for line in content[start_ln:]:
            line = line.strip('\n').strip()
            nums = line.split()
            if len(nums) < 2:
                continue
            x, y = float(nums[0]), float(nums[1])
            ret_x.append(x)
            ret_y.append(y)
    return ret_x, ret_y

def task2():
    fig, axs = plt.subplots(2,2, dpi=300)
    fig.suptitle("Trajectory_comparison")
    fields = ["amber03", "Amber99SB-ILDN", "OPLS", "GROMOS"]
    # fields = ["amber03", "Amber99SB-ILDN"]
    colors = ['r', 'g', 'b', 'violet']; i = 0
    axs[0][0].set_title("hbond",fontsize=10)
    axs[1][0].set_title("gyrate",fontsize=10)
    axs[0][1].set_title("energy",fontsize=10)
    axs[1][1].set_title("sasa",fontsize=10)
    hs = [[] for _ in range(4)]
    for force_field in fields:
        print("Drawing force field {}".format(force_field))
        os.chdir("/mnt/exports/shared/home/gromacs/Gromacs_Result/Task2/optimal/run_{}_4_6".format(force_field))
        # plt.title(force_field)
        case_idx = 0
        os.system("echo \"1 11\" | $GMX hbond -s ./{}.md.tpr -f ./{}.md.xtc -num hbond_{}".format(case_idx, case_idx, force_field))
        x1, y1 = read_xvg("hbond_{}.xvg".format(force_field), 25)
        if i == 0:
            # print(x1)
            # exit(0)
            fac = 1.7
            x1 = [i*fac for i in x1 if i*fac < 1000]
            y1 = y1[:len(x1)]
        h1, = axs[0][0].plot(x1, y1, c=colors[i], label=force_field)
        hs[0].append(h1)

        os.system("echo \"10\" | $GMX energy -s ./{}.md.tpr -f ./{}.md.edr -o energy_{}".format(case_idx, case_idx, force_field))
        x2, y2 = read_xvg("energy_{}.xvg".format(force_field), 24)
        if i == 0:
            fac = 2.5
            x2 = [i*fac for i in x2 if i*fac < 1000]
            y2 = y2[:len(x2)]
        h2, = axs[0][1].plot(x2, y2, c=colors[i], label=force_field)
        hs[1].append(h2)

        os.system("echo \"1\" | $GMX gyrate -s ./{}.md.tpr -f ./{}.md.xtc -o gyrate_{}".format(case_idx, case_idx, force_field))
        x3, y3 = read_xvg("gyrate_{}.xvg".format(force_field), 27)
        if i == 0:
            fac = 1.7
            x3 = [i*fac for i in x3 if i*fac < 1000]
            y3 = y3[:len(x3)]
        # print(x3,y3)
        h3, =axs[1][0].plot(x3, y3, c=colors[i], label=force_field)
        hs[2].append(h3)
        os.system("echo \"1\" | $GMX sasa -s ./{}.md.tpr -f ./{}.md.xtc -o sasa_{}".format(case_idx, case_idx, force_field))
        x4, y4 = read_xvg("sasa_{}.xvg".format(force_field), 24)
        if i == 0:
            fac = 1.7
            x4 = [i*fac for i in x4 if i*fac < 1000]
            y4 = y4[:len(x4)]
        h4, =axs[1][1].plot(x4, y4, c=colors[i], label=force_field)
        os.chdir("/mnt/exports/shared/home/gromacs/Gromacs_Result/Task2/optimal/")
        i += 1
        hs[3].append(h4)
    #plt.legend([h4], [force_field]*4, loc='upper right')
    for h in range(2):
        for w in range(2):
            for i in range(4):
                axs[h][w].legend(hs[i], fields, loc='upper right', fontsize=6)
    plt.tight_layout()
    plt.savefig("output.png")
    axs[0][0].set_xlabel("time", fontsize=10)
    axs[0][0].set_ylabel("hbond", fontsize=10)        
    axs[0][1].set_xlabel("time", fontsize=10)
    axs[0][1].set_ylabel("energy", fontsize=10)
    axs[1][0].set_xlabel("time", fontsize=10)
    axs[1][0].set_ylabel("gyrate", fontsize=10)
    axs[1][1].set_xlabel("time", fontsize=10)
    axs[1][1].set_ylabel("sasa", fontsize=10)
task2()
