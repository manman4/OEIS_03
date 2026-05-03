\\ G.f.: (Sum_{k>=0} binomial(2*k-1,k) * x^k) * (Sum_{k>=0} binomial(2*k,k) * x^k)^3.
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(2*k-1, k)*x^k) * sum(k=0, N, binomial(2*k, k)*x^k)^3)

\\ G.f.: (1/2) * (1/(1-4*x)^(3/2) + 1/(1-4*x)^2).
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(2*k-1, k)*x^k) * sum(k=0, N, binomial(2*k, k)*x^k)^3 - (1/2) * (1/(1-4*x)^(3/2) + 1/(1-4*x)^2))