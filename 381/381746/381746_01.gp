my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, binomial(10*k-1, 2*k)*x^k/k)))

\\ G.f. A(x) satisfies A(x^2) = B(x) * B(-x), where B(x) is the g.f. of A118971.
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, binomial(5*k-1, k)*x^k/k)))
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, binomial(5*k-1, k)*(x^k + (-x)^k)/k)))

\\ G.f.: B(x)^4, where B(x) is the g.f. of A381752.
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, binomial(10*k-1, 2*k)*x^k/k))^(1/4))