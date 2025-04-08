def A(n):
    m = n
    for i in map(int, str(n)):
        if i != 0 and m % i == 0:
            m //= i
    return m
def A382895(n):
    return [A(i) for i in range(1, n + 1)]

n = 10000
ary = A382895(n)
for i in range(1, n + 1):
    print(i, ary[i - 1])
    
