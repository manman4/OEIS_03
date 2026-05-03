\\ G.f.: (Sum_{k>=0} binomial(5*k+2,k) * x^k) * (Sum_{k>=0} binomial(5*k,k) * x^k)^3.
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(5*k+2, k)*x^k) * sum(k=0, N, binomial(5*k, k)*x^k)^3)