M=20;

\\ G.f.: -1/B(x) where B(x) is the g.f. of A052203.
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(4*k+1, k)*x^k))
\\ Expansion of -1/(Sum_{k>=0} binomial(4*k+1,k) * x^k).
my(N=30, x='x+O('x^N)); Vec(-1/sum(k=0, N, binomial(4*k+1, k)*x^k))
