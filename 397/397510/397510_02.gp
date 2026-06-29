\\ Cf. A390239.

\\ G.f.: 1/(g^3 * (1-3*x*g^2) * (1-x*g)) where g = 1+x*g^3 is the g.f. of A001764.
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( 1+x*g^3 - g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( 1/(g^3 * (1-3*x*g^2) * (1-x*g)) )

\\ G.f.: 1/((3-2*g) * (1-g+g^2)) where g = 1+x*g^3 is the g.f. of A001764.
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(3*k, k)/(2*k+1)*x^k)); Vec( 1/((3-2*g) * (1-g+g^2)) - 1/(g^3 * (1-3*x*g^2) * (1-x*g)) )