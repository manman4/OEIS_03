M=20;
\\ Expansion of e.g.f. exp(g^2 - 1), where g = 1+x*g^4 is the g.f. of A002293.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(4*k, k)/(3*k+1)*x^k)); Vec(serlaplace(exp(g^2-1)))


