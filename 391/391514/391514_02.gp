M=20;

\\ Expansion of 1/(g^2 * (2 - g^2)), where g = 1+x*g^3 is the g.f. of A001764
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( 1+x*g^3-g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( 1/(g^2 * (2 - g^2)) )