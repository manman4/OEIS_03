\\ Sum_{k in Z} x^k / (1 - x^(7*k+4)).
my(N=50, x='x+O('x^N)); Vec(sum(k=-N, N, x^(k) / (1 - x^(7*k+4))))

\\ G.f.: Sum_{k in Z} x^(4*k) / (1 - x^(7*k+1)).
my(N=50, x='x+O('x^N)); Vec(sum(k=-N, N, x^(4*k) / (1 - x^(7*k+1))))