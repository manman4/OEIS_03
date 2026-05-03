\\ G.f.: (Sum_{k>=0} binomial(7*k+4,k) * x^k) * (Sum_{k>=0} binomial(7*k,k) * x^k)^3.
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(7*k+4, k)*x^k) * sum(k=0, N, binomial(7*k, k)*x^k)^3)