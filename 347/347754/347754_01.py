from math import isqrt

A347594_list = [1]
N = 10**4
for n in range(1, N+1):
    m = A347594_list[n-1]**2+n**2
    A347594_list.append((isqrt(m)+1)**2-m) 

A347754_list = []
for n in range(1, N+1):
    A347754_list.append(isqrt(A347594_list[n-1]**2 + n**2) + 1)

for i in range(1, N+1):
    print(str(i) + ' ' + str(A347754_list[i-1])) 