M=20;

\\ Expansion of g^6*(4*g-3)/(5-4*g)^4, where g = 1+x*g^5 is the g.f. of A002294.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(5*k, k)/(4*k+1)*x^k)); Vec( g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(5*k, k)/(4*k+1)*x^k)); Vec( 1+x*g^5-g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(5*k, k)/(4*k+1)*x^k)); Vec( g^6*(4*g-3)/(5-4*g)^4 )


