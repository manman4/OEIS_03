\\ G.f.: Sum_{j>=0} (-1)^j*x^(3^j) / Product_{k=0..j} (1 - 2*x^(3^k)).
my(N=30, x='x+O('x^N)); Vec(sum(j=0, logint(N, 3), (-1)^j*x^(3^j) / prod(k=0, j, 1 - 2*x^(3^k)) ))

