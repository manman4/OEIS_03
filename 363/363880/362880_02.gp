my(N=90, x='x+O('x^N)); concat([0, 0], Vec(sum(k=1, N, x^(5*k-2)/(1 - x^(7*k-4)) )))

my(N=90, x='x+O('x^N)); Vec(sum(k=1, N, x^(5*k-2)/(1 - x^(7*k-4)) - x^(3*k)/(1 - x^(7*k-2)) ))