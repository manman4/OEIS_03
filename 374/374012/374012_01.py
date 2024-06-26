from itertools import count
from sympy.solvers.diophantine.diophantine import power_representation

def A374012(n):
    if n == 1: return 1
    for k in count(1):
        try:
            next(power_representation(n, 6, k))
        except:
            continue
        return k


N=10000
for i in range(1, N+1):
    print(i, A374012(i))