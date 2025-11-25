\\ G.f.: g^5 * (1 + 4*x*g + x^2*g^2), where g = 1+x*g^2 is the g.f. of A000108.
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(2*k, k)/(k+1)*x^k)); Vec( g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(2*k, k)/(k+1)*x^k)); Vec( 1+x*g^2 - g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(2*k, k)/(k+1)*x^k)); Vec( g^5 * (1 + 4*x*g + x^2*g^2) )