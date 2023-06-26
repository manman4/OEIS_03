my(N=90, x='x+O('x^N)); concat(0, Vec(sum(k=1, N, x^(5*k-3)/(1 - x^(7*k-5)) )))

my(N=90, x='x+O('x^N)); Vec(sum(k=1, N, x^(5*k-3)/(1 - x^(7*k-5)) - x^(2*k)/(1 - x^(7*k-2)) ))