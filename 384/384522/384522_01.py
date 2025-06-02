from mpmath import polylog

N = 10
# a(n) = (-4)^(n+1)/7 * Li_{-n}(7/3), where Li_{n}(x) is the polylogarithm function.
print([int(round((-4)**(n+1) / 7 * polylog(-n, 7/3))) for n in range(N + 1)])
