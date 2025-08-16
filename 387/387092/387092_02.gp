\\ G.f.: 1/sqrt(1 - x*g^7*(9+g)) where g = 1+x*g^9 is the g.f. of A062994.
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(9*k, k)/(8*k+1)*x^k)); Vec( g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(9*k, k)/(8*k+1)*x^k)); Vec( 1+x*g^9 - g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(9*k, k)/(8*k+1)*x^k)); Vec( 1/sqrt(1 - x*g^7*(9+g)) )