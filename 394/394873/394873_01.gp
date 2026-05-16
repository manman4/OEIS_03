\\ G.f.: q^5 * Sum_{j=0..4} 1 / Product_{k=1..j} (1 - q^k).
my(N=60, x='x+O('x^N)); x^5*sum(j=0, 4, 1/prod(k=1, j, 1-x^k))
my(N=60, x='x+O('x^N)); concat([0, 0, 0, 0, 0], Vec(x^5*sum(j=0, 4, 1/prod(k=1, j, 1-x^k))))