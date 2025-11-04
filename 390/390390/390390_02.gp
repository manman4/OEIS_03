M=20;

\\ G.f.: g^6/((1-7*x*g^6) * (1-x*g)) where g = 1+x*g^7 is the g.f. of A002296.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(7*k, k)/(6*k+1)*x^k)); Vec( g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(7*k, k)/(6*k+1)*x^k)); Vec( 1+x*g^7-g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(7*k, k)/(6*k+1)*x^k)); Vec( g^6/((1-7*x*g^6)*(1-x*g)) )

