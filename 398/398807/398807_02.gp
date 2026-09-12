\\ G.f.: B(x)^4 where B(x) is the g.f. of A079589.
my(N=20, x='x+O('x^N)); Vec(sum(k=0, N, binomial(5*k+1, k)*x^k))
my(N=20, x='x+O('x^N)); Vec(sum(k=0, N, binomial(5*k+1, k)*x^k)^4)