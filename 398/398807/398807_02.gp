\\ G.f.: B(x)^4 where B(x) is the g.f. of A079590.
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(6*k+1, k)*x^k))
my(N=20, x='x+O('x^N)); Vec(sum(k=0, N, binomial(6*k+1, k)*x^k)^4)