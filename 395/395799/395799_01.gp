M=20;

\\ G.f.: -1/B(x) where B(x) is the g.f. of A394270.
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(5*k+2, k)*x^k))
\\ Expansion of -1/(Sum_{k>=0} binomial(5*k+2,k) * x^k).
my(N=30, x='x+O('x^N)); Vec(-1/sum(k=0, N, binomial(5*k+2, k)*x^k))
