def mLPower(m, p, len):
    if p == 0: return [p^k for k in (0..len-1)]
    f = [i/m for i in (1..m-1)]
    h = lambda x: hypergeometric([], f, (x/m)^m)
    g = [v for v in taylor(h(x)^p, x, 0, (len-1)*m).list() if v != 0]
    return [factorial(m*k)*v for (k, v) in enumerate(g)]

for n in (0..6): print(mLPower(2, n, 9))
