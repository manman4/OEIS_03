M=20;
\\ Expansion of e.g.f. exp(g^3 - 1), where g = 1+x*g^2 is the g.f. of A000108.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(2*k, k)/(k+1)*x^k)); Vec(serlaplace(exp(g^3-1)))


