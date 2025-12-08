M=20;

\\ Expansion of g*(1 + x*g), where g = 1+x*g^3 is the g.f. of A001764.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( 1+x*g^3-g )
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( g*(1 + x*g) )

\\ G.f.: 2 - 1/B(x), where B(x) is the g.f. of A390519.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( g*(1 + x*g) - (2 - 1/( g/(1 - x*g^4) )) )

\\ G.f.: 1 + g - 1/g, where g = 1+x*g^4 is the g.f. of A001764.
my(N=M, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( g*(1 + x*g) - (1 + g - 1/g) )