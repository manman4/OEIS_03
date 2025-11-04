M=20;

\\ G.f.: g^7/((1-8*x*g^7) * (1-x*g)) where g = 1+x*g^8 is the g.f. of A007556.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(8*k, k)/(7*k+1)*x^k)); Vec( g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(8*k, k)/(7*k+1)*x^k)); Vec( 1+x*g^8-g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(8*k, k)/(7*k+1)*x^k)); Vec( 1/(1-x*g) )

