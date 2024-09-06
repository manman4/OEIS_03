\\ G.f.: Sum_{k>=1} ( x^k / (1 - k*x^k) )^k.
my(N=60, x='x+O('x^N)); Vec(sum(k=1, N, ( x^k / (1 - k*x^k ) )^k))


