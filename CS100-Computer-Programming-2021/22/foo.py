import os
import random

def gen(l):
    arr = [i for i in range(1, 10001)]
    ret = []
    for i in range(l):
        v = random.choice(arr)
        arr.remove(v)
        ret.append(v)
    return ret

for i in range(1, 5, 1):
    ret = gen(random.randint(4, 20))
    with open("./{}.in".format(i), 'w') as f:
        f.write(str(len(ret)) + '\n')
        f.write(' '.join([str(it) for it in ret]) )
    os.system("./a.out < {}.in > {}.out".format(i, i))

for i in range(5, 11, 1):
    ret = gen(random.randint(4, 999))
    with open("./{}.in".format(i), 'w') as f:
        f.write(str(len(ret)) + '\n')
        f.write(' '.join([str(it) for it in ret]) )
    os.system("./a.out < {}.in > {}.out".format(i, i))