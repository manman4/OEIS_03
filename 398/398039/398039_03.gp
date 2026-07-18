\\ G.f.: 1 + 3 * Sum_{j>=0} x^(2^j) / Product_{k=0..j} (1 - 3*x^(2^k)).
my(N=40, x='x+O('x^N)); Vec(1 + 3 * sum(j=0, logint(N, 2), x^(2^j) / prod(k=0, j, 1 - 3*x^(2^k)) ))

