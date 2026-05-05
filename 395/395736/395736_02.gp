\\ Expansion of (Sum_{k>=0} binomial(5*k,k) * x^k)^5.
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(5*k, k)*x^k))
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(5*k, k)*x^k)^5)