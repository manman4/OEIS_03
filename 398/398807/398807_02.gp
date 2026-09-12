\\ Expansion of (Sum_{k>=0} binomial(5*k+1,k) * x^k)^4.
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(5*k+1, k)*x^k))
my(N=20, x='x+O('x^N)); Vec(sum(k=0, N, binomial(5*k+1, k)*x^k)^4)