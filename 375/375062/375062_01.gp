\\ 1 / Sum_{k in Z} x^k / (1 - x^(5*k+1)).
my(N=50, x='x+O('x^N)); Vec(1/sum(k=-N, N, x^k/(1-x^(5*k+1))))

