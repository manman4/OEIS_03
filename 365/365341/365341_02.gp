\\ E.g.f.: exp( 1/5 * Sum_{k>=1} binomial(5*k,k) * x^k/k ).
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp( 1/5 * sum(k=1, N, binomial(5*k,k)*x^k/k ))))
