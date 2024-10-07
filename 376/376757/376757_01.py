from collections import Counter
def A376757(n): return sum(d*(d+1)>>1 for d in Counter(pow(x, 3, n) for x in range(n)).values())

n = 10000
for i in range(1, n+1):
    print(i, A376757(i))