\\ G.f.: Product_{k>=0} 1/(1 - 3*x^(2^k)).
my(N=40, x='x+O('x^N)); Vec(1/prod(k=0, logint(N, 2), 1-3*x^2^k))


