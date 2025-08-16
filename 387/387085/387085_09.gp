\\ G.f.: B(x)^2/(1 + 2*(B(x)-1)), where B(x) is the g.f. of A000984.
my(N=30, x='x+O('x^N), b=sum(k=0, N, binomial(2*k, k)*x^k)); Vec( b )
my(N=30, x='x+O('x^N), b=sum(k=0, N, binomial(2*k, k)*x^k)); Vec( b^2/(1 + 2*(b-1)) )