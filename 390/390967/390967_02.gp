\\ G.f.: x*g^7 * (1 + 11*x*g + 11*x^2*g^2 + x^3*g^3), where g = 1+x*g^2 is the g.f. of A000108.
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(2*k, k)/(k+1)*x^k)); Vec( g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(2*k, k)/(k+1)*x^k)); Vec( 1+x*g^2 - g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(2*k, k)/(k+1)*x^k)); concat(0, Vec( x*g^7 * (1 + 11*x*g + 11*x^2*g^2 + x^3*g^3) ))