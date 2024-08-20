from itertools import count, islice
from collections import Counter

def A374020_gen(): # generator of terms
    c, s = 0, Counter()
    for n in count(1):
        n2 = n**2
        for i in range(1, n):
            c += s[m:=i**2+n2]
            s[m] += 1
        yield c

n = 1000
A374020_list = list(islice(A374020_gen(), n))

for i, a in enumerate(A374020_list):
    print(f"{i+1} {a}")
