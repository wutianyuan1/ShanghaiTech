################
## 
## File: generate.py
##
##  Generate a random 2-d dataset with Gaussian distribution, in order to serve
##  2-d clustering algorithms testing. Outputs will be written to 'ori.txt'.
##
##  Optinal step: all points assigned to color black, since what we need is the
##  original data, with inner clustering information hidden.
##
##  * Number of colors (i.e. clusters) CANNOT exceed 20.
##
## Jose @ ShanghaiTech University
##
################

from sklearn.datasets.samples_generator import make_blobs
import sys

#
# Open the data file.
#
if len(sys.argv) != 2:
    print("Usage: make gen FILE=<data-file.txt>")
    exit(-1)
fi = open(sys.argv[1], "w")

#
# Ask for dataset parameters. Empty string triggers default values.
#
print("Enter dataset parameters:")
try:
    point_num = int(input(" * # of samples (100,000 ~ 800,000) [default = 450,000]: "))
except ValueError:
    point_num = 450000
try:
    color_num = int(input(" * # of theoretical clusters (1 ~ 20) [default = 15]: "))
except ValueError:
    color_num = 15
try:
    box = float(input(" * |Range| where centers can be (100. ~ 800.) [default = 450.]: "))
except ValueError:
    box = 450.
try:
    std = float(input(" * Standard deviation of a cluster (10. ~ 100.) [default = 50.]: "))
except ValueError:
    std = 50.
if not (100000 <= point_num  <= 800000 and  1  <= color_num <=  20  and
         100.  <=    box     <=  800.  and 10. <=    std    <= 100.):
    raise RuntimeError("Invalid input parameter!")

#
# Generate 150,000 points with 20 Gaussian clusters.
#
data, clist = make_blobs(n_samples=point_num, centers=color_num,
                         cluster_std=std, center_box=(-box, box))

#
# Write the points data to file in a standardized format. The first line
# will specify '<point#> / <color#>', then each following line specifies
# a single data point.
#
fi.write("%d / %d\n" % (point_num, color_num))
for i in range(len(data)):
    fi.write("%.8lf, %.8lf, %d\n" % (data[i][0], data[i][1], 0)) # All black.

#
# Close the data file.
#
fi.close()
