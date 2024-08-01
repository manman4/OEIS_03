\\ Sum_{k in Z} x^k / (1 - x^(7*k+2)).
my(N=50, x='x+O('x^N)); Vec(sum(k=-N, N, x^(k) / (1 - x^(7*k+2))))
