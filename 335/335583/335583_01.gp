\\ G.f.: x^4 * Sum_{j=0..3} 1 / Product_{k=1..j} (1 - x^k).
my(N=60, x='x+O('x^N)); x^4*sum(j=0, 3, 1/prod(k=1, j, 1-x^k))
my(N=60, x='x+O('x^N)); concat([0, 0, 0, 0], Vec(x^4*sum(j=0, 3, 1/prod(k=1, j, 1-x^k))))