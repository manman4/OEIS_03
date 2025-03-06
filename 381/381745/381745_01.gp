my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, binomial(8*k-1, 2*k)*x^k/k)))

\\ G.f. A(x) satisfies A(x^2) = B(x)/x * B(-x)/(-x), where B(x) is the g.f. of A006632.
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, binomial(4*k-1, k)*x^k/k)))
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, binomial(4*k-1, k)*(x^k + (-x)^k)/k)))

\\ G.f.: B(x)^3, where B(x) is the g.f. of A381751.
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, binomial(8*k-1, 2*k)*x^k/k))^(1/3))