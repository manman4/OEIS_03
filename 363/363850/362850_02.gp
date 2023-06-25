my(N=90, x='x+O('x^N)); Vec(sum(k=1, N, x^(4*k-3)/(1 - x^(7*k-6)) ))

my(N=90, x='x+O('x^N)); Vec(sum(k=1, N, x^(4*k-3)/(1 - x^(7*k-6)) - x^k/(1 - x^(7*k-3)) ))