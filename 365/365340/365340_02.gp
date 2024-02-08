\\ E.g.f.: exp( 1/4 * Sum_{k>=1} binomial(4*k,k) * x^k/k ).
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp( 1/4 * sum(k=1, N, binomial(4*k,k)*x^k/k ))))


