my(N=90, x='x+O('x^N)); Vec(sum(k=1, N, x^(5*k-4)/(1 - x^(7*k-6)) ))

my(N=90, x='x+O('x^N)); Vec(sum(k=1, N, x^(5*k-4)/(1 - x^(7*k-6)) - x^k/(1 - x^(7*k-2)) ))