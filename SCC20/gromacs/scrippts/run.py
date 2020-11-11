import sys, os

verbose = sys.argv[1]
arg_combs = [(4,4), (4,5), (4,6), (6,4), (8, 3), (12, 2)]
for (ntmpi, ntomp) in arg_combs:
    cmd = "./protein.sh %d %d " % (ntmpi, ntomp)
    cmd += verbose
    os.system(cmd + "> runlog.txt")
    dirname = "run-{}-{}".format(ntmpi, ntomp)
    os.mkdir(dirname)
    os.system("cp ./em.log ./"+dirname)
    os.system("cp ./pr.log ./"+dirname)
    os.system("cp ./md.log ./"+dirname)
    os.system("cp ./runlog.txt ./"+dirname)

