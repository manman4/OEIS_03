\\ G.f.: q^6 * Sum_{j=0..5} (6-j) * q^j / Product_{k=1..j} (1 - q^k).
my(N=60, x='x+O('x^N)); x^6*sum(j=0, 5, (6-j)*x^j/prod(k=1, j, 1-x^k))
my(N=60, x='x+O('x^N)); concat([0, 0, 0, 0, 0, 0], Vec(x^6*sum(j=0, 5, (6-j)*x^j/prod(k=1, j, 1-x^k))))