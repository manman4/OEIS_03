\\ G.f.: x*(B(x)/x)^2 where B(x) is the g.f. of A025174.
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(3*k+2, k)*x^k))
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(3*k+2, k)*x^k)^2)