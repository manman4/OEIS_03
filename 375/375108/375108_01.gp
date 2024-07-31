\\ Sum_{k in Z} x^(3*k) / (1 - x^(7*k+3)).
my(N=50, x='x+O('x^N)); Vec(sum(k=-N, N, x^(3*k) / (1 - x^(7*k+3))))


