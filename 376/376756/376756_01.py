def A376756(n):
    c = 0
    for x in range(n):
        z = x**2%n
        for y in range(x, n):
            if not (z+y*(x+y))%n:
                c += 1
    return c 

n = 10000
# i A376756(1)で出力
for i in range(1, n+1):
    print(i, A376756(i))