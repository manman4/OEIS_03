\\ G.f.: Sum_{j>=0} (-1)^j*x^(4^j) / Product_{k=0..j} (1 - 2*x^(4^k)).
my(N=40, x='x+O('x^N)); Vec(sum(j=0, logint(N, 4), (-1)^j*x^(4^j) / prod(k=0, j, 1 - 2*x^(4^k)) ))

