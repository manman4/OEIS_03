\\ G.f.: Product_{k>=0} 1/(1 - 2*x^(3^k)).
my(N=40, x='x+O('x^N)); Vec(1/prod(k=0, logint(N, 3), 1-2*x^3^k))


