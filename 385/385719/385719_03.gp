\\ G.f.: g/sqrt((2-g) * (6-5*g)) where g = 1+x*g^6 is the g.f. of A002295.
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(6*k, k)/(5*k+1)*x^k)); Vec( g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(6*k, k)/(5*k+1)*x^k)); Vec( 1+x*g^6 - g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(6*k, k)/(5*k+1)*x^k)); Vec( g/sqrt((2-g) * (6-5*g)) )