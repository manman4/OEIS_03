\\ Sum_{k in Z} x^(2*k) / (1 - x^(7*k+2)).
my(N=50, x='x+O('x^N)); Vec(sum(k=-N, N, x^(2*k) / (1 - x^(7*k+2))))

