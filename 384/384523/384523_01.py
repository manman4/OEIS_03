from mpmath import polylog

N = 10
# a(n) = (-3)^(n+1)/7 * Li_{-n}(7/4), where Li_{n}(x) is the polylogarithm function.
print([int(round((-3)**(n+1) / 7 * polylog(-n, 7/4))) for n in range(N + 1)])
