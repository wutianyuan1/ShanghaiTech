d = list(range(1, 366))
import random

s = 0
for _ in range(100000):
    l = []
    while True:
        item = random.choice(d)
        if item in l:
            break
        l.append(item)
    s += len(l)+1

print(s/100000.0)