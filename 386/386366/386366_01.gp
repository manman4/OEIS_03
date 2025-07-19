\\ Expansion of e.g.f. (Sum_{k>=0} binomial(4*k,k) * x^k)^(1/4).

my(N=20, x='x+O('x^N)); Vec(serlaplace(sum(k=0, N, binomial(4*k, k)*x^k)^(1/4)))
