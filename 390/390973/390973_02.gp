\\ G.f.: x*g^8 * (1 + 11*x*g^2 + 11*x^2*g^4 + x^3*g^6), where g = 1+x*g^3 is the g.f. of A001764.
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( 1+x*g^3 - g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); concat(0, Vec( x*g^8 * (1 + 11*x*g^2 + 11*x^2*g^4 + x^3*g^6) ))