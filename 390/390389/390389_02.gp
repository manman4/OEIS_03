M=20;

\\ G.f.: g^5/((1-6*x*g^5) * (1-x*g)) where g = 1+x*g^6 is the g.f. of A002295.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(6*k, k)/(5*k+1)*x^k)); Vec( g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(6*k, k)/(5*k+1)*x^k)); Vec( 1+x*g^6-g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(6*k, k)/(5*k+1)*x^k)); Vec( g^5/((1-6*x*g^5)*(1-x*g)) )

