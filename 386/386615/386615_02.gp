\\ G.f.: g^2 * (g-1)/(6-5*g)^2 where g=1+x*g^6.

my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(6*k, k)/(5*k+1)*x^k)); 1+x*g^6 - g
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(6*k, k)/(5*k+1)*x^k)); concat(0, Vec(g^2*(g-1)/(6-5*g)^2))
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(6*k, k)/(5*k+1)*x^k)); g^2*(g-1)/(6-5*g)^2