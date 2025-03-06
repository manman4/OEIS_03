my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, binomial(5*k-1, 2*k-1)*x^k/k)))

\\ G.f.: B(x)^2, where B(x) is the g.f. of A060941.
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, binomial(5*k-1, 2*k-1)*x^k/k))^(1/2))