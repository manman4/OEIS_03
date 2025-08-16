\\ G.f.: sqrt( g/(12-11*g) ) where g = 1+x*g^12. 
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(12*k, k)/(11*k+1)*x^k)); Vec( g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(12*k, k)/(11*k+1)*x^k)); Vec( 1+x*g^12 - g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(12*k, k)/(11*k+1)*x^k)); Vec( sqrt( g/(12-11*g) ) )
