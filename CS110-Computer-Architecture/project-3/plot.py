################
##
## File: plot.py
##
##  Plot the 2-d data points in standardized 'res.txt' ascii files, utlizing
##  the `matplotlib.pyplot` scatter plots.
##
##  * Number of colors (i.e. clusters) CANNOT exceed 20.
##
## Jose @ ShanghaiTech University
##
################

import numpy as np
import matplotlib.pyplot as plt
import os, sys

#
# Backup color set for 20 different clusters.
#
colorset = ["black"    , "gray"   , "lightcoral"   , "maroon"   , "indigo",
            "chocolate", "gold"   , "lightseagreen", "lawngreen", "olive" ,
            "cyan"     , "skyblue", "navy"         , "blue"     , "red"   ,
            "violet"   , "hotpink", "lightpink"    , "azure"    , "beige" ]

#
# Open the original data file and the clustered output.
#
if len(sys.argv) != 2:
    print("Usage: make plot FILE=<result.txt>")
    exit(-1)
if not (os.path.isfile(sys.argv[1])):
    raise RuntimeError("Data file "+sys.argv[1]+" does not exist!")
fo = open(sys.argv[1])
pn, cn = tuple(map(lambda s: int(s), fo.readline().split("/")))

#
# Read in data points from both files.
#
data = np.array([(float(l.split(", ")[0]), float(l.split(", ")[1]))
                 for l in fo.readlines()])

#
# Read in corresponding colors.
#
fo.seek(0); fo.readline()
clist = [colorset[int(l.split(", ")[2])] for l in fo.readlines()]

#
# Plot the points on 2-d axis with colors.
#
print("Plotting might be laggish, please be patient...", end='')
sys.stdout.flush()
plt.figure(figsize=(11, 5))
plt.subplot(1, 2, 1)    # Original.
plt.scatter(data[:,0], data[:,1], c='black', s=1)
plt.title("Original Distribution")
plt.subplot(1, 2, 2)    # Result.
plt.scatter(data[:,0], data[:,1], c= clist , s=1)
plt.title("Result of K-Means Clustering")
plt.show()
print("done.")
