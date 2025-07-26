\\ G.f.: g*(g-1)/(4-3*g)^2 where g=1+x*g^4.

my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(4*k, k)/(3*k+1)*x^k)); 1+x*g^4 - g
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(4*k, k)/(3*k+1)*x^k)); concat(0, Vec(g*(g-1)/(4-3*g)^2))
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(4*k, k)/(3*k+1)*x^k)); g*(g-1)/(4-3*g)^2