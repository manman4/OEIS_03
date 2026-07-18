\\ G.f.: 1 + 2 * Sum_{j>=0} x^(3^j) / Product_{k=0..j} (1 - 2*x^(3^k)).
my(N=40, x='x+O('x^N)); Vec(1 + 2 * sum(j=0, logint(N, 3), x^(3^j) / prod(k=0, j, 1 - 2*x^(3^k)) ))

