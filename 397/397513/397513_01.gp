\\ G.f.: 1/(g^3 * (1-7*x*g^6) * (1-x*g^5)) where g = 1+x*g^7 is the g.f. of A002296.
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(7*k, k)/(6*k+1)*x^k)); Vec( g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(7*k, k)/(6*k+1)*x^k)); Vec( 1+x*g^7 - g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(7*k, k)/(6*k+1)*x^k)); Vec( 1/(g^3*(1-7*x*g^6)*(1-x*g^5)) )