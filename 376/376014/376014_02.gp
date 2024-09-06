\\ G.f.: Sum_{k>=1} (k*x^k)^k / (1 - x^k)^(k+1).
my(N=60, x='x+O('x^N)); Vec(sum(k=1, N, (k*x^k)^k / (1 - x^k)^(k+1)))

