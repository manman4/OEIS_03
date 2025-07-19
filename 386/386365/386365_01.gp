\\ Expansion of e.g.f. (Sum_{k>=0} binomial(3*k,k) * x^k)^(1/3).

my(N=30, x='x+O('x^N)); Vec(serlaplace(sum(k=0, N, binomial(3*k, k)*x^k)^(1/3)))
