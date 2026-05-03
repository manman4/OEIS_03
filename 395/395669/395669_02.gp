M=20;

\\ Expansion of g^7*(4-3*g)/(7-6*g)^2, where g = 1+x*g^7 is the g.f. of A002296.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(7*k, k)/(6*k+1)*x^k)); Vec( g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(7*k, k)/(6*k+1)*x^k)); Vec( 1+x*g^7-g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(7*k, k)/(6*k+1)*x^k)); Vec( g^7*(4-3*g)/(7-6*g)^2 )



