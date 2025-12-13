M=20;
\\ Expansion of e.g.f. exp(g^4 - 1), where g = 1+x*g^3 is the g.f. of A001764.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec(serlaplace(exp(g^4-1)))


